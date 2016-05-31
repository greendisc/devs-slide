/*
 * =====================================================================================
 *
 *       Filename:  sim-launcher.cc
 *    Description:  Launches DC simulator and controls execution
 *
 * =====================================================================================
 */

#include <string>
#include <vector>
#include <list>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "cpprest/basic_types.h"
#include <sys/time.h>
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include <tclap/CmdLine.h>

#include "alloc/allocator.h"
#include "dc-factory.h"
#include "dc-simulator.h"
#include "generic_defs.h"
#include "parser/dc-parser.h"
#include "weather/weather.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;
using namespace web::json;

// Global definitions
json::value config;
std::ifstream weatherfile;
std::ofstream powerfile;
int udpsock;
std::ifstream jobfile;
bool noLog;

/*-------------------------------------
 * Helper functions
 *-------------------------------------*/
void loadConfig(DCParser::CmdLineArgs &cmdArgs)
{
    // Opening configuration file
    std::ifstream configfile(cmdArgs.configFile);
    if ( !configfile ) {
        LOG_FATAL << "[==DC SIMULATOR] Configuration file " << configfile
                  << " could not be opened. Exiting...";
    }
    std::stringstream cfg;
    cfg << configfile.rdbuf();
    config = json::value::parse(cfg);
    configfile.close(); 
    
    // Opening weather file
    weatherfile.open(cmdArgs.weatherFile);
    if ( !weatherfile ) {
        LOG_FATAL << "[==DC SIMULATOR] Weather file " << weatherfile
                  << " could not be opened. Exiting...";
    }

    // Opening output power file
    powerfile.open(cmdArgs.powerOutFile);
    if ( !powerfile ){
        LOG_FATAL << "[==DC SIMULATOR] Power summary output file " << powerfile
                  << " could not be opened. Exiting...";
    }
    
    // Connecting socket
    if (cmdArgs.offline){
        LOG_INFO << "[==DC SIMULATOR] Off-line mode, will open job logger file";
        jobfile.open(cmdArgs.jobLoggerFile);
        if (!jobfile){
            LOG_FATAL << "[==DC SIMULATOR] Could not open job logger file in off-line mode";
        }
    }
    else{
        LOG_INFO << "[==DC SIMULATOR] Online mode, will create socket for communication";
        int val=1;
        struct sockaddr_in sin;
        udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        setsockopt(udpsock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
        
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(cmdArgs.port);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        if ( bind(udpsock, (struct sockaddr *) &sin, sizeof(sin)) < 0 ){
            LOG_FATAL << "Error when binding udp socket";
        }
        LOG_INFO << "Socket succesfully created at port " << cmdArgs.port;
    }
}

int recvJobMessage(DCSimulator &sim, Allocator &alloc)
{
    int bytecnt;
    char cbuf[200];

    VLOG_2 << "Will wait for next message";
    memset(cbuf, 0, sizeof(cbuf));
    bytecnt=recv(udpsock, cbuf, 200, 0);
    if (bytecnt <=0){
        LOG_FATAL << "Error when reading from socket";
    }
    VLOG_2 << "Bytecnt is: " <<bytecnt << " and buffer: " << cbuf;
    //Parsing received buffer: "type,jobtime,jobid"
    std::string buffer = std::string(cbuf);
    VLOG_1 << "Received buffer is " << buffer ;
    int jobId = alloc.parseNewJob(sim, buffer);
    
    return jobId;
}

int getJobLoggerLine(DCSimulator &sim, Allocator &alloc)
{
    
    std::string buffer;
    if (!std::getline(jobfile, buffer)){
        LOG_WARNING << "Could not read line from jobfile. We should NOT have reached"
                    << " this point in the code.";
        return -1;
    }
    VLOG_1 << "Job line is " << buffer ;
    int jobId = alloc.parseNewJob(sim, buffer);
    if (jobId == 0){
        // In off-line mode, this function is not the one signaling the end of
        // execution (exit only signals end of file).
        // In this case we go back to the beginning of the file again and restore
        // the initial allocation
        LOG_INFO << "Reached end-of-file. Will go back to the beginning";
        jobfile.clear();
        jobfile.seekg(0, std::ios::beg);
        VLOG_2 << "Resetting room parameters";
        DCParser::addRoomParams(config, sim);
        VLOG_2 << "Resetting allocation";
        DCParser::addWorkload(config, sim);
        VLOG_2 << "Seeking new job (first line)";
        noLog=true;
        if (!std::getline(jobfile, buffer)){
            LOG_WARNING << "Could not read job file!";
            return -1;
        }
        jobId = alloc.parseNewJob(sim, buffer);
    }
    return jobId;
}
    
/*
 *--------------------------------------------------------------------------------------
 * MAIN: DC SIMULATOR LAUNCHER
 *--------------------------------------------------------------------------------------
 */
int main (int argc, char* argv[])
{
    try { 

        // Logging
		google::InitGoogleLogging(argv[0]);

        // Should parse command-line args here!
        //-------------------------------------
        DCParser::CmdLineArgs cmdArgs;
        DCParser::parseCmdLineArgs(argc, argv, cmdArgs);
        
        std::string FSpolicy = "server-proactive"; //fixed
        std::string coolPolicy = cmdArgs.coolPolicy ; //fixed, budget
        
        // Loading initial configuration
        loadConfig(cmdArgs);
                       
        // Parsing initial configuration
        // ----------------------------------------
        DCSimulator::DCParams layout;
        DCParser::parseDCConfig(config, layout);
        VLOG_1 << "Layout parsed. Constructing simulator";
        
        //==================================================
		DCSimulator sim(layout);
        LOG_INFO << "Simulator created succesfully";
        //==================================================

        VLOG_1 << "Adding room parameters...";
        DCParser::addRoomParams(config, sim);
        LOG_INFO << "Parameters inserted into simulator";

        //---------------------------------------------------
        VLOG_1 << "Adding room parameters...";
        // FIXME-marina: in the future we'll need to change workload parsing
        DCParser::addWorkload(config, sim);
        LOG_INFO << "Workload inserted into simulator";

        // Creating allocator
        //====================
        Allocator alloc(cmdArgs.baseTime, cmdArgs.firstSubmit, cmdArgs.allocPath, cmdArgs.offline);
        Weather weather(weatherfile);

        // Creating cooling control policy
        CoolingPolicy* cool = PolicyFactory::createCoolingPolicy(coolPolicy, sim);
        if (cool == NULL){
            LOG_FATAL << "Cooling policy does not exist!";
        }
        
        //=================================================
        // Run simulator main loop
        //=================================================
        bool fi=false;
        VLOG_2 << "Will now print current allocation";
        alloc.printCurrentAlloc(sim);

        // Check begin flag to see if job is starting or ending
        std::function<int(DCSimulator&, Allocator&)> getNewJob;
        if (cmdArgs.offline){
            VLOG_1 << "Off-line mode. Will call getJobLoggerLine.";
            getNewJob = std::bind(getJobLoggerLine, std::placeholders::_1, std::placeholders::_2);
        }
        else{
            VLOG_1 << "On-line mode. Will use udp sockets.";
            getNewJob = std::bind(recvJobMessage, std::placeholders::_1, std::placeholders::_2);
        }

        while (!fi){
            noLog=false;
            int jobId = getNewJob(sim, alloc);
            VLOG_1 << "NoLog flag is: " << noLog;
            if (jobId < 0){
                LOG_FATAL << "Error processing job";
            }
            else if (jobId == 0){
                LOG_INFO << "Received exit code";
                break;
            }
            double tempOut = weather.getCurrentWeather(alloc.getCurrentTime(), fi);
            VLOG_1 << "Current time is: " << alloc.getCurrentTime()
                   << " -- outside temperature: " << tempOut
                   << " -- jobId: " << jobId ;
            //Chaging workload allocation
            alloc.updateAllocation(sim, jobId);
            //Printing new workload allocation to file
            alloc.printCurrentAlloc(sim);
                        
            // For this allocation, run a cooling policy
            // and compute the power
            VLOG_1 << "Running cooling policy with outdoor temperature " << tempOut;
            cool->run(tempOut);
            VLOG_2 << "Computing power consumption";
            sim.computePower(tempOut);

            VLOG_1 << "Printing all data";
            sim.printAllPowerValues(alloc.getCurrentTime(), tempOut, powerfile);
        }

        weatherfile.close();
        powerfile.close();

        LOG_INFO << "[==DC Simulator] Computing energy statistics";
        // TODO-marina: add function to compute montly statistics
        sim.printEnergyStatistics(cmdArgs.powerOutFile);
	}
    catch (std::exception& e)  {
        LOG_FATAL << "[==DC Simulator] Exception: " << e.what() ;
    }

    LOG_INFO << "[==DC Simulator] Program exiting" ;
    return 0;   
}

