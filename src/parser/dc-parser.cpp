/*
 * ===================================================================
 *
 *       Filename:  parser.cc
 *    Description:  Data center Layout, Params and workload parser 
 *
 * ===================================================================
 */

#include "dc-parser.h"

bool DCParser::parseCmdLineArgs(int argc, char *argv[], CmdLineArgs &parsedArgs)
{
	// Get the value parsed by each arg and fill-in struct.
	parsedArgs.configFile = "test/ConfigFile.xml";
	parsedArgs.weatherFile = "WeatherFile.txt";
	parsedArgs.powerOutFile = "PowerFile.txt";
	parsedArgs.port = 4321;
	parsedArgs.baseTime = 1316242565;
	parsedArgs.offline = false;
	if (parsedArgs.offline){
		parsedArgs.jobLoggerFile = "JobLogger.txt";
	}
	parsedArgs.firstSubmit = FIRST_SUBMISSION_TIME;
	parsedArgs.allocPath.clear();
	parsedArgs.coolPolicy = "fixed";
    return true;
}

void DCParser::parseDCConfig(DCSimulator::DCParams &layout)
{
	xercesc::XMLPlatformUtils::Initialize();
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
    parser->parse("test/ConfigFile.xml");
    xercesc::DOMDocument* doc = parser->getDocument();
    xercesc::DOMNode* dataCenter = doc->getFirstChild();

    VLOG_1 << "Got a Data Center layout. Going to parse it";

    bool res = DCParser::parseLayout(dataCenter, layout);
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
    xercesc::XMLPlatformUtils::Terminate();
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

        if (!couple.has_field("equipment")){
            LOG_WARNING << "RackAndIRC has no equipment info!";
            return false;
        }
        json::array &equipment = couple[U("equipment")].as_array();
        if (!equipment.size()){
            LOG_WARNING << "RackAndIRC has no racks inside!";
            return false;
        }
        thisCouple.numRacksPerIRC = equipment.size();
        VLOG_1 << "RackAndIRC couple " << thisCouple.coupleName << " has "
               << equipment.size() << " racks per IRC" ;
        for (auto j=0; j<equipment.size(); j++){
            Rack::RackParams rack;
            json::value &rackval = equipment.at(j);
            if (!parseRackAndServers(rackval, rack)){
                LOG_WARNING << "Could not parse rack and servers";
                return false;
            }
            thisCouple.racks.push_back(rack);
        }
        rackIRC.push_back(thisCouple);
    }
    return true;
}

bool DCParser::parseRackAndServers(xercesc::DOMNode* rackInfo, Rack::RackParams &rack)
{
    if (!rackInfo.has_field("Rack")){
        LOG_WARNING << "This is no rack!";
        return false;
    }
    json::value &rackContents = rackInfo[U("Rack")];

    //Rack name
    if (!rackContents.has_field("name")){
        LOG_WARNING << "This is no name for rack!";
        return false;
    }
    rack.rackName = rackContents[U("name")].as_string();
    VLOG_1 << "Got rack " << rack.rackName ;
    
    //Servers inside
    if (!rackContents.has_field("Servers")){
        LOG_WARNING << "There are no servers in rack!";
        return false;
    }
    json::array serverArray = rackContents[U("Servers")].as_array();
    for (auto i=0; i<serverArray.size(); i++){
        json::array &thisServer = serverArray.at(i).as_array();
        std::string name = thisServer.at(0).as_string();
        std::string type = thisServer.at(1).as_string();
        VLOG_1 << " -- Got server " << name << " of type " << type;
        rack.servers.insert(std::make_pair(name, type));
    }
    
    return true;
}

bool DCParser::addRoomParams(xercesc::DOMNode* dataCenter, DCSimulator &sim)
{
    if (!config.has_field(U("RoomParams"))){
        LOG_FATAL << "No data center initial configuration parameters";
    }
    json::value &params = config[U("RoomParams")];
    
    // IRC Inlet temperature
    //-----------------------
    if (!params.has_field(U("IRCAirTemp"))){
        LOG_WARNING << "No IRC Air temperature vector";
        return false;
    }
    json::array &irc = params[U("IRCAirTemp")].as_array();
    VLOG_2 << "IRC has array size of " << irc.size();
    for (unsigned int i=0; i< irc.size(); i++){
        json::array &data = irc.at(i).as_array();
        std::string rackIrcName = data.at(0).as_string();
        int temp = data.at(1).as_integer();

        //Set inlet temperature to all servers affected by this IRC
        VLOG_2 << "Will set " << temp << " temperature to rack "
               << rackIrcName;
        sim.getRoom()->setCoupleInletTemp(rackIrcName, temp);
    }

    // Server fan speed
    //-----------------
    if (!params.has_field(U("ServerFanSpeed"))){
        LOG_WARNING << "No server fan speed temperature vector";
        return false;
    }
    json::array &fan = params[U("ServerFanSpeed")].as_array();
    VLOG_2 << "Server fan speed has array size of " << fan.size();
    for (unsigned int i=0; i< fan.size(); i++){
        json::array &data = fan.at(i).as_array();
        std::string rackName = data.at(0).as_string();
        std::string serverName = data.at(1).as_string();
        int fanspeed = data.at(2).as_integer();

        //Set fan speed to all servers in the list
        VLOG_2 << "Will set fan speed " << fanspeed << " rpm in server "
               << serverName << " at rack " << rackName;
        sim.getRoom()->setFanSpeed(rackName, serverName, fanspeed);        
    }
    
    return true;
}

bool DCParser::addWorkload(xercesc::DOMNode* dataCenter, DCSimulator &sim)
{
    if (!config.has_field(U("Workload"))){
        LOG_FATAL << "No initial workload parameters";
    } 
    json::value &wkload = config[U("Workload")];
    
    if (!wkload.has_field(U("ServerPower"))){
        LOG_WARNING << "No server power vector";
        return false;
    }
    json::array &power = wkload[U("ServerPower")].as_array();
    VLOG_2 << "Server power has array size of " << power.size();
    for (unsigned int i=0; i< power.size(); i++){
        json::array &data = power.at(i).as_array();
        std::string rackName = data.at(0).as_string();
        std::string serverName = data.at(1).as_string();
        int numCores = data.at(2).as_integer();
        double cpuPower = data.at(3).as_double();
        double memPower = data.at(4).as_double();

        // Set power to all servers
        sim.getRoom()->setWorkloadPower(rackName, serverName, cpuPower, memPower, numCores);
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
