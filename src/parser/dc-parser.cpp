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
    try {
        TCLAP::CmdLine cmd("DC power and energy simulator", ' ', "0.9");

        TCLAP::ValueArg<std::string> configFile("c","config","DC configuration file",true,
                                                "config.txt","string");
        cmd.add(configFile);
        
        TCLAP::ValueArg<std::string> weatherFile("w","weather","DC weather info file",true,
                                                 "weather.txt","string");
        cmd.add(weatherFile);

        TCLAP::ValueArg<std::string> powerFile("e","power","DC power summary output file",true,
                                                 "power.txt","string");
        cmd.add(powerFile);

        TCLAP::ValueArg<int> port("p","port","UDP receiver port",true,1234,"integer");
        cmd.add(port);

        TCLAP::ValueArg<time_t> baseTime("t","time","Simulator base time",true,0,"time");
        cmd.add(baseTime);
        
        TCLAP::ValueArg<time_t> firstSubmit("f","first","First real job submission time",false,0,"time");
        cmd.add(firstSubmit);

        // Off-line mode
        TCLAP::SwitchArg offlineSwitch("o","offline","DCsim runs in offline mode", false);
        cmd.add(offlineSwitch);

        TCLAP::ValueArg<std::string> jobLogger("j", "joblogger", "File where job allocations are logged",
                                               false, "joblogger.txt", "string");
        cmd.add(jobLogger);

        TCLAP::ValueArg<std::string> allocPath("a", "allocpath", "File where job allocations are logged",
                                               false, "/tmp/", "string");
        cmd.add(allocPath);

        TCLAP::ValueArg<std::string> coolPolicy("y", "coolpolicy", "Cooling policy",
                                               false, "/tmp/", "string");
        cmd.add(coolPolicy);
        
        //Switch example
        //TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);
        
        // Parse the argv array.
        cmd.parse( argc, argv );
        
        // Get the value parsed by each arg and fill-in struct. 
        parsedArgs.configFile = configFile.getValue();
        parsedArgs.weatherFile = weatherFile.getValue();
        parsedArgs.powerOutFile = powerFile.getValue();
        parsedArgs.port = port.getValue();
        parsedArgs.baseTime = baseTime.getValue();
        parsedArgs.offline = offlineSwitch.getValue();
        if (parsedArgs.offline){
            if(jobLogger.getValue().empty()){
                LOG_FATAL << "Offline switch is on, but no joblogger file provided";
            }
            parsedArgs.jobLoggerFile = jobLogger.getValue();
        }
        if (!firstSubmit.getValue()){
            parsedArgs.firstSubmit = FIRST_SUBMISSION_TIME;
        }
        else {
            parsedArgs.firstSubmit = firstSubmit.getValue();
        }
        if (allocPath.getValue().empty()){
            parsedArgs.allocPath.clear();
        }
        else {
            parsedArgs.allocPath = allocPath.getValue();
        }
        if (coolPolicy.getValue().empty()){
            parsedArgs.coolPolicy = "fixed";
        }
        else {
            parsedArgs.coolPolicy = coolPolicy.getValue();
        }
    }
    catch (TCLAP::ArgException &e) {
        LOG_FATAL << "Error parsing cmd line args: " << e.error()
                  << " for arg " << e.argId();
    }
    
    return true;
}

void DCParser::parseDCConfig(json::value &config, DCSimulator::DCParams &layout)
{
    if (config.is_null()){
        LOG_FATAL << "DC json configuration is null. Cannot parse";
    }
    if (!config.has_field(U("DataCenter"))){
        LOG_FATAL << "No data center layout configuration";
    }
        
    json::array &dcArray = config[U("DataCenter")].as_array();
    if (dcArray.size() != 1){
        LOG_FATAL << "Data Center field of size " << dcArray.size()
                  << " is not supported at the moment. Exiting.";
    }
    VLOG_1 << "Got a Data Center layout. Going to parse it";

    bool res = DCParser::parseLayout(dcArray.at(0), layout);
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
    
}

bool DCParser::parseLayout (json::value &layout, DCSimulator::DCParams &params)
{    
    // DC Name
    if (!layout.has_field(U("name"))){
        LOG_WARNING << "No name field!";
        return false;
    }
    params.dcName = layout[U("name")].as_string();

    // Chiller --> external parsing
    if (!layout.has_field(U("Chiller"))){
        LOG_WARNING << "No name chiller!";
        return false;
    }
    json::value chiller = layout[U("Chiller")];
    if (!parseChiller(chiller, params.chiller)){
        LOG_WARNING << "Wrong chiller config...";
        return false;
    }

    // Pump
    if (!layout.has_field(U("Pump"))){
        LOG_WARNING << "No pump field!";
        return false;
    }
    params.pumpType = layout[U("Pump")].as_string();

    // Room
    if (!layout.has_field(U("Room"))){
        LOG_WARNING << "No room field!";
        return false;
    }
    params.roomType = layout[U("Room")].as_string();

    // IRC (optional)
    if (layout.has_field(U("IRC"))){
        LOG_INFO << "IRC field, DC contains IRC coolers";
        params.ircType = layout[U("IRC")].as_string();
    }

    // IT --> external parsing
    if (!layout.has_field(U("IT"))){
        LOG_WARNING << "No IT field!";
        return false;
    }
    json::array itArray = layout[U("IT")].as_array();
    if (!params.ircType.empty()){
        VLOG_2 << "Got the IT array, and it should have IRCs";
        if (!parseRacksAndIRCs(itArray, params.rackIRC)){
            LOG_WARNING << "Wrong IT config...";
            return false;
        }
        return true;
    }
    VLOG_2 << "Data Center without IRC's. Not implemented.";
    return false;    
}

bool DCParser::parseChiller(json::value &config, Chiller::ChillerParams &chiller)
{
    if (!config.has_field(U("type"))){
        LOG_WARNING << "No chiller type!";
        return false;
    }
    chiller.chillerType = config[U("type")].as_string();
    
    if (!config.has_field(U("cold-water-temp"))){
        LOG_WARNING << "No chiller cold water temperature!";
        return false;
    }
    chiller.coldWaterTemp = config[U("cold-water-temp")].as_double();

    if (!config.has_field(U("outdoor-temp"))){
        LOG_WARNING << "No outdoor temperature!";
        return false;
    }
    chiller.outdoorTemp = config[U("outdoor-temp")].as_double();
    
    return true;
}

bool DCParser::parseRacksAndIRCs(json::array &itArray,
                                 std::vector<RackAndIRC::RackAndIRCParams> &rackIRC )
{
    VLOG_1 << "Found " << itArray.size() << " RackAndIRC Couples";
    for (auto i=0; i< itArray.size(); i++){
        // Get the RackIRCCouple data
        json::value &element = itArray.at(i);
        if (!element.has_field(U("RackIRCCouple"))){
            LOG_WARNING << "No RackIRCCouple found!! This is an error";
            return false;
        }
        json::value &couple = element[U("RackIRCCouple")];

        // Parse RackIRCCouple parameters
        RackAndIRC::RackAndIRCParams thisCouple;
        if (!couple.has_field("name")){
            LOG_WARNING << "RackAndIRC has no name!";
            return false;
        }
        thisCouple.coupleName = couple[U("name")].as_string();
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

bool DCParser::parseRackAndServers(json::value &rackInfo, Rack::RackParams &rack)
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

bool DCParser::addRoomParams(json::value &config, DCSimulator &sim)
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

bool DCParser::addWorkload(json::value &config, DCSimulator &sim)
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

