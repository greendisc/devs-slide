/*
 * ===================================================================
 *
 *       Filename:  parser.cc
 *    Description:  Data center Layout, Params and workload parser 
 *
 * ===================================================================
 */

#include "dc-parser.h"

bool DCParser::parseCmdLineArgs(CmdLineArgs &parsedArgs)
{
	// Get the value parsed by each arg and fill-in struct.
	parsedArgs.configFile = "test/ConfigFile.xml";
	parsedArgs.weatherFile = "test/WeatherFile.txt";
	parsedArgs.powerOutFile = "test/PowerFile.txt";
	parsedArgs.port = 4321;
	parsedArgs.baseTime = 1316242565;
	parsedArgs.offline = true;
	if (parsedArgs.offline){
		parsedArgs.jobLoggerFile = "JobLogger.txt";
	}
	parsedArgs.firstSubmit = FIRST_SUBMISSION_TIME;
	parsedArgs.allocPath.clear();
	parsedArgs.coolPolicy = "fixed";
    return true;
}

xercesc::DOMDocument* DCParser::parseDCConfig(DCSimulator::DCParams &layout)
{
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
    parser->parse("test/ConfigFile.xml");
    xercesc::DOMDocument* xmlConfigFile = parser->getDocument();
    xercesc::DOMNode* xmlDataCenter = xmlConfigFile->getFirstChild();

    VLOG_1 << "Got a Data Center layout. Going to parse it";

    bool res = DCParser::parseLayout(xmlDataCenter, layout);
    if (!res){
        LOG_FATAL << "Could not parse DC layout. Exiting...";
    }
        
    VLOG_2 << "DC Parameter are:" << std::endl
           << " -- DcName = " << layout.dcName << std::endl
           << " -- Chiller => type= " << layout.chiller.chillerType 
           << " coldWaterTemp= " << layout.chiller.coldWaterTemp << std::endl
           << " -- Pump= " << layout.pumpType << std::endl
           << " -- Room= " << layout.roomType << std::endl
           << " -- IRC= " << layout.ircType ;
    return xmlConfigFile;
}

bool DCParser::parseLayout (xercesc::DOMNode* xmlDataCenter, DCSimulator::DCParams &params) {
    // DC Name
	xercesc::DOMNode* xmlName = DCParser::xmlDameNodoHijo(xmlDataCenter, "name");
	params.dcName = xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());

    // Chiller --> external parsing
	xercesc::DOMNode* xmlChiller = DCParser::xmlDameNodoHijo(xmlDataCenter, "Chiller");
    if (xmlChiller == 0){
        LOG_WARNING << "No name chiller!";
        return false;
    }
    if (!parseChiller(xmlChiller, params.chiller)){
        LOG_WARNING << "Wrong chiller config...";
        return false;
    }

    // Pump
    xercesc::DOMNode* xmlPump = DCParser::xmlDameNodoHijo(xmlDataCenter, "Pump");
    if (xmlPump==0){
        LOG_WARNING << "No pump field!";
        return false;
    }
    params.pumpType = xercesc::XMLString::transcode(xmlPump->getFirstChild()->getNodeValue());

    // Room
    xercesc::DOMNode* xmlRoom = DCParser::xmlDameNodoHijo(xmlDataCenter, "Room");
    if (xmlRoom==0) {
        LOG_WARNING << "No room field!";
        return false;
    }
    params.roomType = xercesc::XMLString::transcode(xmlRoom->getFirstChild()->getNodeValue());

    // IRC (optional)
    xercesc::DOMNode* xmlIRC = DCParser::xmlDameNodoHijo(xmlDataCenter, "IRC");
    if (xmlIRC==0) {
        LOG_INFO << "IRC field, DC contains IRC coolers";
        params.ircType = xercesc::XMLString::transcode(xmlIRC->getFirstChild()->getNodeValue());
    }

    // IT --> external parsing
    xercesc::DOMNode* xmlIT = DCParser::xmlDameNodoHijo(xmlDataCenter, "IT");
    if (xmlIT==0) {
        LOG_WARNING << "No IT field!";
        return false;
    }
    xercesc::DOMNodeList* xmlITs = xmlDataCenter->getOwnerDocument()->getElementsByTagName(xercesc::XMLString::transcode("IT"));
    if (xmlITs->getLength()>0){
        VLOG_2 << "Got the IT array, and it should have IRCs";
        if (!parseRacksAndIRCs(xmlITs, params.rackIRC)){
            LOG_WARNING << "Wrong IT config...";
            return false;
        }
        return true;
    }
    VLOG_2 << "Data Center without IRC's. Not implemented.";
    return false;    
}

bool DCParser::parseChiller(xercesc::DOMNode* xmlChiller, Chiller::ChillerParams &chiller) {
	xercesc::DOMNode* xmlType = DCParser::xmlDameNodoHijo(xmlChiller, "type");
    if (xmlType==0){
        LOG_WARNING << "No chiller type!";
        return false;
    }
    chiller.chillerType = xercesc::XMLString::transcode(xmlType->getFirstChild()->getNodeValue());
    
    xercesc::DOMNode* xmlColdWaterTemp = DCParser::xmlDameNodoHijo(xmlChiller, "cold-water-temp");
    if (xmlColdWaterTemp==0){
        LOG_WARNING << "No chiller cold water temperature!";
        return false;
    }
    chiller.coldWaterTemp = std::stod(xercesc::XMLString::transcode(xmlColdWaterTemp->getFirstChild()->getNodeValue()));

    xercesc::DOMNode* xmlOutdoorTemp = DCParser::xmlDameNodoHijo(xmlChiller, "outdoor-temp");
    if (xmlOutdoorTemp==0) {
        LOG_WARNING << "No outdoor temperature!";
        return false;
    }
    chiller.outdoorTemp = std::stod(xercesc::XMLString::transcode(xmlOutdoorTemp->getFirstChild()->getNodeValue()));
    
    return true;
}

bool DCParser::parseRacksAndIRCs(xercesc::DOMNodeList* xmlITs, std::vector<RackAndIRC::RackAndIRCParams> &rackIRC )
{
    VLOG_1 << "Found " << xmlITs->getLength() << " RackAndIRC Couples";
    for (unsigned int i=0; i< xmlITs->getLength(); i++){
        // Get the RackIRCCouple data
    	xercesc::DOMNode* xmlIT = xmlITs->item(i);
    	xercesc::DOMNode* xmlRackIRCCouple = DCParser::xmlDameNodoHijo(xmlIT, "RackIRCCouple");

        // Parse RackIRCCouple parameters
        xercesc::DOMNode* xmlName = DCParser::xmlDameNodoHijo(xmlRackIRCCouple, "name");
        RackAndIRC::RackAndIRCParams thisCouple;
        thisCouple.coupleName = xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());
        VLOG_1 << "Couple name is " << thisCouple.coupleName;

        xercesc::DOMNode* xmlEquipment = DCParser::xmlDameNodoHijo(xmlRackIRCCouple, "equipment");
        if (xmlEquipment==0){
            LOG_WARNING << "RackAndIRC has no equipment info!";
            return false;
        }

        xercesc::DOMNodeList* xmlRacks = xmlEquipment->getChildNodes();
        if (xmlRacks->getLength()==0){
            LOG_WARNING << "RackAndIRC has no racks inside!";
            return false;
        }
        VLOG_1 << "RackAndIRC couple " << thisCouple.coupleName << " has " << xmlRacks->getLength() << " racks per IRC" ;
        for (unsigned int j=0; j<xmlRacks->getLength(); j++){
        	xercesc::DOMNode* xmlRack = xmlRacks->item(j);
            Rack::RackParams rackParams;
            if (!parseRackAndServers(xmlRack, rackParams)){
                LOG_WARNING << "Could not parse rack and servers";
                return false;
            }
            thisCouple.racks.push_back(rackParams);
        }
        rackIRC.push_back(thisCouple);
    }
    return true;
}

bool DCParser::parseRackAndServers(xercesc::DOMNode* xmlRack, Rack::RackParams &rackParams) {
	xercesc::DOMNode* xmlName = DCParser::xmlDameNodoHijo(xmlRack, "name");
	//Rack name
    if (xmlName == 0){
        LOG_WARNING << "This is no name for rack!";
        return false;
    }
    rackParams.rackName = xercesc::XMLString::transcode(xmlName->getFirstChild()->getNodeValue());
    VLOG_1 << "Got rack " << rackParams.rackName ;
    
    //Servers inside
    xercesc::DOMNodeList* xmlServers = xmlRack->getChildNodes();
    unsigned int numServers = 0;
    for (unsigned int i=1; i<xmlServers->getLength(); i+=2){
    	xercesc::DOMNode* xmlServer1 = xmlServers->item(i);
    	xercesc::DOMNode* xmlServer2 = xmlServers->item(i+1);
        std::string name = xercesc::XMLString::transcode(xmlServer1->getFirstChild()->getNodeValue());
        std::string type = xercesc::XMLString::transcode(xmlServer2->getFirstChild()->getNodeValue());
        VLOG_1 << " -- Got server " << name << " of type " << type;
        rackParams.servers.insert(std::make_pair(name, type));
        numServers++;
    }
    
    if (numServers == 0) {
        LOG_WARNING << "There are no servers in rack!";
        return false;
    }

    return true;
}

bool DCParser::addRoomParams(xercesc::DOMDocument* xmlConfigFile, DCSimulator &sim) {
	xercesc::DOMNodeList* xmlRoomParamsAsList = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("RoomParams"));
    if (xmlRoomParamsAsList->getLength()==0){
        LOG_FATAL << "No data center initial configuration parameters";
        return false;
    }
    
    xercesc::DOMNode* xmlRoomParams = xmlRoomParamsAsList->item(0);
    xercesc::DOMNodeList* xmlRoomParamsChildren = xmlRoomParams->getChildNodes();

    unsigned int currentChildPos = 0;
    xercesc::DOMNode* xmlCurrentChild = xmlRoomParamsChildren->item(currentChildPos);
    std::string xmlCurrentChildNodeName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());

    // IRC Inlet temperature
    //-----------------------
    if (xmlCurrentChildNodeName!="IRCAirTemp"){
        LOG_WARNING << "No IRC Air temperature vector";
        return false;
    }
    while(currentChildPos<xmlRoomParamsChildren->getLength() && xmlCurrentChildNodeName=="IRCAirTemp") {
    	xercesc::DOMNode* xmlNextChild = xmlRoomParamsChildren->item(currentChildPos++);
    	std::string rackName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
    	int rackTemp = std::stoi(xercesc::XMLString::transcode(xmlNextChild->getFirstChild()->getNodeValue()));
        //Set inlet temperature to all servers affected by this IRC
        VLOG_2 << "Will set " << rackTemp << " temperature to rack " << rackName;
        sim.getRoom()->setCoupleInletTemp(rackName, rackTemp);
        if(currentChildPos<xmlRoomParamsChildren->getLength()) {
        	xmlCurrentChild = xmlRoomParamsChildren->item(currentChildPos);
        	xmlCurrentChildNodeName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
        }
    }

    // Server fan speed
    //-----------------
    if (xmlCurrentChildNodeName!="ServerFanSpeed"){
        LOG_WARNING << "No server fan speed temperature vector";
        return false;
    }
    while(currentChildPos<xmlRoomParamsChildren->getLength() && xmlCurrentChildNodeName=="ServerFanSpeed") {
    	xercesc::DOMNode* xmlSecondChild = xmlRoomParamsChildren->item(currentChildPos++);
    	xercesc::DOMNode* xmlThirdChild = xmlRoomParamsChildren->item(currentChildPos++);
    	std::string rackName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
    	std::string serverName = xercesc::XMLString::transcode(xmlSecondChild->getFirstChild()->getNodeValue());
    	int fanSpeed = std::stoi(xercesc::XMLString::transcode(xmlThirdChild->getFirstChild()->getNodeValue()));
    	//Set fan speed to all servers in the list
    	VLOG_2 << "Will set fan speed " << fanSpeed << " rpm in server " << serverName << " at rack " << rackName;
        sim.getRoom()->setFanSpeed(rackName, serverName, fanSpeed);
        if(currentChildPos<xmlRoomParamsChildren->getLength()) {
        	xmlCurrentChild = xmlRoomParamsChildren->item(currentChildPos);
        	xmlCurrentChildNodeName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
        }
    }
    return true;
}

bool DCParser::addWorkload(xercesc::DOMDocument* xmlConfigFile, DCSimulator &sim) {
	xercesc::DOMNodeList* xmlWorkloadAsList = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("Workload"));
    if (xmlWorkloadAsList->getLength()==0){
        LOG_FATAL << "No initial workload parameters";
        return false;
    } 
    
    xercesc::DOMNode* xmlWorkload = xmlWorkloadAsList->item(0);
    xercesc::DOMNodeList* xmlWorkloadChildren = xmlWorkload->getChildNodes();

    unsigned int currentChildPos = 0;
    xercesc::DOMNode* xmlCurrentChild = xmlWorkloadChildren->item(currentChildPos);
    std::string xmlCurrentChildNodeName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
    if (xmlCurrentChildNodeName!="ServerPower"){
    	LOG_WARNING << "No server power vector";
        return false;
    }
    while(currentChildPos<xmlWorkloadChildren->getLength() && xmlCurrentChildNodeName=="ServerPower") {
    	xercesc::DOMNode* xmlChild2 = xmlWorkloadChildren->item(currentChildPos++);
    	xercesc::DOMNode* xmlChild3 = xmlWorkloadChildren->item(currentChildPos++);
    	xercesc::DOMNode* xmlChild4 = xmlWorkloadChildren->item(currentChildPos++);
    	xercesc::DOMNode* xmlChild5 = xmlWorkloadChildren->item(currentChildPos++);
    	std::string rackName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
    	std::string serverName = xercesc::XMLString::transcode(xmlChild2->getFirstChild()->getNodeValue());
    	int numCores = std::stoi(xercesc::XMLString::transcode(xmlChild3->getFirstChild()->getNodeValue()));
    	double cpuPower = std::stod(xercesc::XMLString::transcode(xmlChild4->getFirstChild()->getNodeValue()));
    	double memPower = std::stod(xercesc::XMLString::transcode(xmlChild5->getFirstChild()->getNodeValue()));
        // Set power to all servers
        sim.getRoom()->setWorkloadPower(rackName, serverName, cpuPower, memPower, numCores);
    	if(currentChildPos<xmlWorkloadChildren->getLength()) {
    		xmlCurrentChild = xmlWorkloadChildren->item(currentChildPos);
    		xmlCurrentChildNodeName = xercesc::XMLString::transcode(xmlCurrentChild->getFirstChild()->getNodeValue());
    	}
    }
    return true;
}

xercesc::DOMNode* DCParser::xmlDameNodoHijo(xercesc::DOMNode* padre, std::string nombreHijo) {
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

std::string DCParser::xmlDameAtributo(xercesc::DOMNode* nodo, std::string atributo) {
    if (nodo->getAttributes()->getNamedItem(xercesc::XMLString::transcode(atributo.c_str())) == 0)
        return "";
    return xercesc::XMLString::transcode(nodo->getAttributes()->getNamedItem(xercesc::XMLString::transcode(atributo.c_str()))->getNodeValue());
}
