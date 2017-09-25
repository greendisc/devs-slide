/*
 * DevsSlide.cpp
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#include "DevsSlide.h"

DevsSlide::DevsSlide(std::string OutFile, std::string JobFile, std::string WeatherFile, std::string configFile,double initialTime) : Coupled("DevsSlide") {

	std::map<std::string, int> ircInlets;
	std::map<std::string, std::map<std::string, double[4]>> serverFanSpeeds;
	//Get the structure
	std::map<std::string, std::map<std::string, std::list<std::string>>> dataCenterStructure = parseXmlStructure(configFile, ircInlets, serverFanSpeeds);


	// 1.- Jobs generator
	/*
	 * Los objetos no se declaran como atributos, ya que no será necesario acceder a ellos.
	 * La destructora de Coupled, se encargará de liberar la memoria.
	 */
	DevsJobsGenerator* jobs = new DevsJobsGenerator(initialTime,"Jobs", JobFile);
	//agregamos cada componente al acoplado
	Coupled::addComponent(jobs);


	/*
	 * Generamos una lista de IRC's para manejar más facilmente las conexiones
	 */
	std::list<std::string> ircNames;
	for(std::map<std::string, std::map<std::string, std::list<std::string>>>::iterator itr = dataCenterStructure.begin(); itr!=dataCenterStructure.end(); ++itr) {
		ircNames.push_back(itr->first);
	}


	//Energy Calculator
	DevsEnergyCalculator* energyCalculator= new DevsEnergyCalculator("EnergyCalculator", ircNames, OutFile);
	Coupled::addComponent(energyCalculator);

	//Al igual que con JobsGenerator, agregamos el componente, pero esta vez conectamos los componentes
	//Cooling
	DevsCooling* cooling = new DevsCooling(ircNames);
	Coupled::addComponent(cooling);
	Coupled::addCoupling(cooling, &(cooling->oPumpPower), energyCalculator, &(energyCalculator->iPumpPower));
	Coupled::addCoupling(cooling, &(cooling->oChillerPower), energyCalculator, &(energyCalculator->iChillerPower));
	Coupled::addCoupling(cooling, &(cooling->oTowerPower), energyCalculator, &(energyCalculator->iTowerPower));
	Coupled::addCoupling(cooling, &(cooling->oWeatherTemp), energyCalculator, &(energyCalculator->iWeatherTemp));

	//Weather
	DevsWeather* weather = new DevsWeather("Weather", WeatherFile);
	Coupled::addComponent(weather);
	Coupled::addCoupling(weather, &(weather->oOut), cooling, &(cooling->iWeatherTemp));
	//Señal de fin fichero de weather...
	Coupled::addCoupling(weather, &(weather->oStop), jobs, &(jobs->iStop));
	//Room
	DevsRoom* room= new DevsRoom(dataCenterStructure, ircInlets, serverFanSpeeds, ircNames);
	Coupled::addComponent(room);

	Coupled::addCoupling(jobs, &(jobs->oOut), room, &(room->iJob));
	Coupled::addCoupling(cooling, &cooling->oColdWaterTemp, room, &room->iWaterTemp);
	/*
	 * conectamos todas las salidas del acoplado Room iterando sobre los IRC's
	 */
	for(std::list<std::string>::iterator itr = ircNames.begin(); itr!=ircNames.end(); ++itr){
		Coupled::addCoupling(room, room->oIRCGallons[*itr], cooling, cooling->iIRCGallons[*itr]);
		Coupled::addCoupling(room, room->oIRCHotWater[*itr], cooling, cooling->iIRCHotWaterTemp[*itr]);
		Coupled::addCoupling(room, room->oIRCPower[*itr],energyCalculator, energyCalculator->iIRCPower[*itr]);
		Coupled::addCoupling(room, room->oIRCITPower[*itr],energyCalculator, energyCalculator->iIRCITPower[*itr]);
		Coupled::addCoupling(room, room->oIRCFSPower[*itr],energyCalculator, energyCalculator->iIRCFSPower[*itr]);
		Coupled::addCoupling(room, room->oIRCAvgTempCpu[*itr],energyCalculator, energyCalculator->iIRCAvgTempCpu[*itr]);
		Coupled::addCoupling(room, room->oIRCMaxTempCpu[*itr],energyCalculator, energyCalculator->iIRCMaxTempCpu[*itr]);
	}//Para conocer la configuracion comun de todos los puertos, chequear los comentarios de DevsIrc.cpp

}

DevsSlide::~DevsSlide() {
	std::list<Component*> components = this->getComponents();
	for(std::list<Component*>::iterator itr = components.begin(); itr!=components.end(); ++itr) {
		delete *itr;
	}
}

std::map<std::string, std::map<std::string, std::list<std::string>>> DevsSlide::parseXmlStructure(const std::string& xmlFile, std::map<std::string, int>& ircParams, std::map<std::string, std::map<std::string, double[4]>>& serverFanSpeeds) {
	std::map<std::string, std::map<std::string, std::list<std::string>>> dataCenterStructure;

	xercesc::XMLPlatformUtils::Initialize();
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
    parser->parse(xmlFile.c_str());
    xercesc::DOMDocument* xmlConfigFile = parser->getDocument();

    // Obtenemos los IRC
    xercesc::DOMNodeList* xmlRackIrcCouples = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("RackIRCCouple"));
    for(unsigned int i=0; i<xmlRackIrcCouples->getLength(); ++i) {
    	// Obtenemos el nombre del IRC
    	xercesc::DOMNode* xmlRackIrcCouple = xmlRackIrcCouples->item(i);
        xercesc::DOMNode* xmlName = DevsSlide::xmlDameNodoHijo(xmlRackIrcCouple, "name");
        std::string ircName = xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());
        std::cout << "IRC name: " << ircName.c_str() << std::endl;
        dataCenterStructure[ircName] = std::map<std::string, std::list<std::string>>();
        // Iteramos sobre los racks
        xercesc::DOMNodeList* xmlEquipments = DevsSlide::xmlDameNodosHijos(xmlRackIrcCouple, "equipment");
        // Equipments representa cada Rack del IRC:
        for(unsigned int j=0; j<xmlEquipments->getLength(); ++j) {
        	xercesc::DOMNode* xmlEquipment = xmlEquipments->item(j);
        	xercesc::DOMNode* xmlRack = DevsSlide::xmlDameNodoHijo(xmlEquipment, "Rack");
        	xercesc::DOMNode* xmlName = DevsSlide::xmlDameNodoHijo(xmlRack, "name");
        	std::string rackName = xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());
        	// Iteramos sobre los servers del rack
        	std::cout << "Rack name: " << rackName.c_str() << std::endl;
        	dataCenterStructure[ircName][rackName] = std::list<std::string>();
            xercesc::DOMNodeList* xmlServers = DevsSlide::xmlDameNodosHijos(xmlRack, "Servers");
            for (unsigned int k=0; k<xmlServers->getLength(); k+=2){
            	xercesc::DOMNode* xmlServer1 = xmlServers->item(k);
            	xercesc::DOMNode* xmlServer2 = xmlServers->item(k+1);
                std::string serverName = xercesc::XMLString::transcode(xmlServer1->getFirstChild()->getNodeValue());
                std::string serverType = xercesc::XMLString::transcode(xmlServer2->getFirstChild()->getNodeValue());
                std::cout << "Server name: " << serverName.c_str() << std::endl;
                dataCenterStructure[ircName][rackName].push_back(serverName);
            }
        }

    }
    //IRC's Inlet
    xercesc::DOMNodeList* xmlRoomParams = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("IRCAirTemp"));
    for (unsigned int i = 0; i < xmlRoomParams->getLength(); i+=2) {
    	xercesc::DOMNode* xmlName = xmlRoomParams->item(i);
    	xercesc::DOMNode* xmlValue = xmlRoomParams->item(i+1);
    	std::string ircName= xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());
    	std::string s= xercesc::XMLString::transcode(xmlValue->getFirstChild()->getNodeValue());
    	int ircInlet = std::stoi(s);
    	ircParams[ircName]=ircInlet;
	}
    //Initial Fan speeds
    xercesc::DOMNodeList* xmlServerFanSpeed = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("ServerFanSpeed"));
    for (unsigned int i = 0; i < xmlServerFanSpeed->getLength(); i+=3) {
    	xercesc::DOMNode* xmlRack= xmlServerFanSpeed->item(i);
    	xercesc::DOMNode* xmlServer = xmlServerFanSpeed->item(i+1);
    	xercesc::DOMNode* xmlFS = xmlServerFanSpeed->item(i+2);
    	std::string rack= xercesc::XMLString::transcode(xmlRack->getFirstChild()->getNodeValue());
    	std::string server= xercesc::XMLString::transcode(xmlServer->getFirstChild()->getNodeValue());
    	std::string s= xercesc::XMLString::transcode(xmlFS->getFirstChild()->getNodeValue());
    	int fanSpeed = std::stoi(s);
    	serverFanSpeeds[rack][server][0]=fanSpeed;
	}
    //Initial Workloads
    xercesc::DOMNodeList* xmlServerPower = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("ServerPower"));
    for (unsigned int i = 0; i < xmlServerPower->getLength(); i+=5) {
    	xercesc::DOMNode* xmlRack= xmlServerPower->item(i);
    	xercesc::DOMNode* xmlServer = xmlServerPower->item(i+1);
    	xercesc::DOMNode* xmlnumCores = xmlServerPower->item(i+2);
    	xercesc::DOMNode* xmlCpuPower = xmlServerPower->item(i+3);
    	xercesc::DOMNode* xmlMempower = xmlServerPower->item(i+4);
    	std::string rack= xercesc::XMLString::transcode(xmlRack->getFirstChild()->getNodeValue());
    	std::string server= xercesc::XMLString::transcode(xmlServer->getFirstChild()->getNodeValue());
    	std::string s= xercesc::XMLString::transcode(xmlnumCores->getFirstChild()->getNodeValue());
    	double numcores = std::stod(s);
    	s= xercesc::XMLString::transcode(xmlCpuPower->getFirstChild()->getNodeValue());
    	double cpuPower = std::stod(s);
    	s= xercesc::XMLString::transcode(xmlMempower->getFirstChild()->getNodeValue());
    	double memPower = std::stod(s);
    	serverFanSpeeds[rack][server][1]=numcores;
    	serverFanSpeeds[rack][server][2]=cpuPower;
    	serverFanSpeeds[rack][server][3]=memPower;

	}
    xercesc::XMLPlatformUtils::Terminate();
    return dataCenterStructure;
}

xercesc::DOMNode* DevsSlide::xmlDameNodoHijo(xercesc::DOMNode* padre, std::string nombreHijo) {
	xercesc::DOMNodeList* lista_hijos = padre->getChildNodes();
	xercesc::DOMNode* itr = 0;
    std::string itr_nombre;
    for (unsigned int i=0; i<lista_hijos->getLength(); i++)
    {
        itr = lista_hijos->item(i);
        if (itr) itr_nombre = xercesc::XMLString::transcode(itr->getNodeName());
        if (itr_nombre == nombreHijo) return itr;
    }
    return 0;
}

xercesc::DOMNodeList* DevsSlide::xmlDameNodosHijos(xercesc::DOMNode* padre, std::string nombreHijo) {
    xercesc::DOMElement* xmlPadreAsElement = dynamic_cast<xercesc::DOMElement*>(padre);
    xercesc::DOMNodeList* xmlHijos = xmlPadreAsElement->getElementsByTagName(xercesc::XMLString::transcode(nombreHijo.c_str()));
    return xmlHijos;
}

