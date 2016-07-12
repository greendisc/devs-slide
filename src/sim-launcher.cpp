/*
 * =====================================================================================
 *
 *       Filename:  sim-launcher.cc
 *    Description:  Launches DC simulator and controls execution
 *
 * =====================================================================================
 */

#include "sim-launcher.h"

SimLauncher::SimLauncher() {
	xmlConfigFile = 0;
	xercesc::XMLPlatformUtils::Initialize();
}

SimLauncher::~SimLauncher() {
    xercesc::XMLPlatformUtils::Terminate();
}


/*-------------------------------------
 * Helper functions
 *-------------------------------------*/
void SimLauncher::loadConfig(DCParser::CmdLineArgs &cmdArgs)
{
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
        LOG_INFO << "[==DC SIMULATOR] Online mode NOT SUPPORTED";
    }
}

int recvJobMessage(DCSimulator &sim, Allocator &alloc)
{
    /*int bytecnt;
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
    
    return jobId;*/
	return -1;
}

int SimLauncher::getJobLoggerLine(DCSimulator &sim, Allocator &alloc)
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
        DCParser::addRoomParams(xmlConfigFile, sim);
        VLOG_2 << "Resetting allocation";
        DCParser::addWorkload(xmlConfigFile, sim);
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
//std::string FSpolicy = "server-proactive"; //fixed
//std::string coolPolicy = cmdArgs.coolPolicy ; //fixed, budget
int main (int argc, char* argv[])
{
    try { 

    	SimLauncher* simLauncher = new SimLauncher();
        // Should parse command-line args here!
        //-------------------------------------
        DCParser::CmdLineArgs cmdArgs;
        DCParser::parseCmdLineArgs(cmdArgs);
        // Loading initial configuration
        simLauncher->loadConfig(cmdArgs);
                       
        // Parsing initial configuration
        // ----------------------------------------
        DCSimulator::DCParams layout;
        xercesc::DOMDocument* xmlConfigFile = DCParser::parseDCConfig(layout);
        simLauncher->xmlConfigFile = xmlConfigFile;
        VLOG_1 << "Layout parsed. Constructing simulator";
        
        //==================================================
		DCSimulator sim(layout);
        LOG_INFO << "Simulator created succesfully";
        //==================================================

        VLOG_1 << "Adding room parameters...";
        DCParser::addRoomParams(xmlConfigFile, sim);
        LOG_INFO << "Parameters inserted into simulator";

        //---------------------------------------------------
        VLOG_1 << "Adding room parameters...";
        // FIXME-marina: in the future we'll need to change workload parsing
        DCParser::addWorkload(xmlConfigFile, sim);
        LOG_INFO << "Workload inserted into simulator";

        // Creating allocator
        //====================
        Allocator alloc(cmdArgs.baseTime, cmdArgs.firstSubmit, cmdArgs.allocPath, cmdArgs.offline);
        Weather weather(simLauncher->weatherfile);

        // Creating cooling control policy
        CoolingPolicy* cool = PolicyFactory::createCoolingPolicy(cmdArgs.coolPolicy, sim);
        if (cool == NULL){
            LOG_FATAL << "Cooling policy does not exist!";
        }
        
        //=================================================
        // Run simulator main loop
        //=================================================
        bool fi=false;
        VLOG_2 << "Will now print current allocation";
        alloc.printCurrentAlloc(sim);

        while (!fi){
            noLog=false;
            int jobId = simLauncher->getJobLoggerLine(sim, alloc);
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
            sim.printAllPowerValues(alloc.getCurrentTime(), tempOut, simLauncher->powerfile);
        }

        simLauncher->weatherfile.close();
        simLauncher->powerfile.close();

        LOG_INFO << "[==DC Simulator] Computing energy statistics";
        // TODO-marina: add function to compute montly statistics
        sim.printEnergyStatistics(cmdArgs.powerOutFile);
        delete simLauncher;
	}
    catch (std::exception& e)  {
        LOG_FATAL << "[==DC Simulator] Exception: " << e.what() ;
    }

    LOG_INFO << "[==DC Simulator] Program exiting" ;
    return 0;   
}

