/*
 * ===================================================================
 *
 *       Filename:  allocator.cc
 *    Description:  Allocator simulator 
 *
 * ===================================================================
 */

#include "allocator.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include "../generic_defs.h"

constexpr char Allocator::ALLOCOUT_FILE_PATH[];
constexpr char Allocator::ALLOCOUT_FILE_PREFIX[];
constexpr char Allocator::ALLOCOUT_FILE_SUFFIX[];
constexpr char Allocator::JOBPARAM_FILE_PREFIX[];
constexpr char Allocator::JOBPARAM_FILE_SUFFIX[];
constexpr char Allocator::CURRENT_ALLOC_FILE[];

extern bool noLog;

Allocator::Allocator(time_t baseTime, time_t firstSubmit, std::string &allocPath, bool offline)
    : currentTime(0),
      m_baseTime(baseTime),
      m_firstSubmit(firstSubmit),
      offline(offline),
      firstRound(true),
      m_lastRoundTime(0),
      m_allocPath(allocPath)
{
    if (m_allocPath.empty()){
        m_allocPath= ALLOCOUT_FILE_PATH;
    }
}

Allocator::~Allocator()
{
}

time_t Allocator::getCurrentTime()
{
    return currentTime;
}

void Allocator::setCurrentTime(time_t time)
{
    currentTime = (time - m_baseTime);
}

double Allocator::addPower(double op1, double op2)
{
    return (op1+op2);
}
double Allocator::subtractPower(double op1, double op2)
{
    return (op1-op2);
}

void Allocator::printCurrentAlloc(DCSimulator &sim)
{
    //Here we only access all the servers in the DC 
    //and print the information on power
    VLOG_2 << "Printing current allocation to " << CURRENT_ALLOC_FILE ;
    std::ofstream ofs;
    ofs.open (CURRENT_ALLOC_FILE, std::ofstream::out | std::ofstream::trunc);
    std::vector<Nodes> allNodes;
    sim.getRoom()->getWorkloadAlloc(allNodes);
    for (unsigned int i=0; i< allNodes.size(); i++){
        ofs << "[\"" << allNodes[i].ircName << "\",\"" << allNodes[i].serverName << "\","
            << allNodes[i].numCores << "," << allNodes[i].cpuPower << ","
            << allNodes[i].memPower << "]" << std::endl;
    }
    ofs.close();
    VLOG_2 << "File printed successfully";
}

int Allocator::parseNewJob(DCSimulator &sim, std::string &buffer)
{
    Job job;
    std::stringstream ss(buffer);
    std::string it;
    
    // Parsing type
    if (!std::getline(ss,it,';')){
        LOG_ERROR << "UDP job begin/end recv parse error!";
        return -1;
    }
    VLOG_2 << "data is: " << it;
    if (it.compare("exit") == 0){
        LOG_INFO << "Received an exit message";
        return 0;
    }
    if (it.compare("jobbegin") == 0){
        job.begin = true;
    }
    else {
        job.begin=false;
    }
    // Parsing jobtime
    if (!std::getline(ss,it,';')){
        LOG_ERROR << "UDP job time recv parse error!";
        return -1;
    }
    time_t thisTime = boost::lexical_cast<time_t>(it.c_str());
    if (thisTime < (m_firstSubmit+m_baseTime)){
        VLOG_1 << "Thistime is less than first submit time. Not logging";
        noLog=true;            
    }
    if (!offline){
        setCurrentTime(thisTime);
    }
    else {
        // If I am in offline mode, I need to track the rounds
        if (thisTime < (getCurrentTime()+m_baseTime)){
            VLOG_1 << "Previous time was " << getCurrentTime()
                   << " and now is lower " << thisTime
                   << ". This is not the first round";
            firstRound = false;
            m_lastRoundTime = getCurrentTime();
        }
        if (firstRound){
            setCurrentTime(thisTime);
        }
        else {
            setCurrentTime(thisTime + m_lastRoundTime);
        }
    }

    if (job.begin){
        //Job does not exist, I'll push it into the map
        job.startTime = getCurrentTime();
        job.endTime=0;
    }
    else {
        //Job already exists, I need to pick it from queue
        //(to update data)
        job.endTime = getCurrentTime();
    }
    //Parsing jobid
    if (!std::getline(ss,it,';')){
        LOG_ERROR << "UDP job id recv parse error!";
        return -1;
    }
    job.jobId= atoi(it.c_str());   
    
    // Now checking if job exists
    if (!job.begin){
        //Job is ending (must exist)
        std::map<int, Job>::iterator jobIt;
        jobIt = m_jobMap.find(job.jobId);
        if (jobIt == m_jobMap.end()){
            LOG_ERROR << "Job is finishing and I did not find it in job map!";
            return -1;
        }
        jobIt->second.endTime = job.endTime;
        jobIt->second.begin = false;

        //XXX-marina: if we uncomment the following lines, we won't be
        // able to launch DCsim in off-line mode
        
        // Removing file /tmp/alloc_out_${jobid}.txt
        // std::string rmcmd = "rm /tmp/alloc_out_"
        //     + boost::lexical_cast<std::string>(job.jobId) + ".txt";
        // system(rmcmd.c_str());

        // Returning
        return job.jobId;
    }
    VLOG_1 << "Printing job parameters: job.begin=" << job.begin
           << " -- job.startTime=" << job.startTime << " -- job.endTime=" << job.endTime
           << " -- job.jobId=" << job.jobId ;

    // Job is beginning
    //-------------------
    // Now reading remaining data from file.(/tmp/alloc_out_${jobid}.txt)
    // Here we expect each line to be: ["rack1", "s1", numthreads]
    std::ifstream fs(m_allocPath + ALLOCOUT_FILE_PREFIX
                     + boost::lexical_cast<std::string>(job.jobId)
                     + ALLOCOUT_FILE_SUFFIX);
    if ( !fs ) {
        LOG_FATAL << "[==DC SIMULATOR] Job allocation file could not be opened. Exiting...";
    }
    std::stringstream jobfile;
    //Inserting braces at the beginning and end to make a json file
    jobfile << "{ \"Allocation\": [" << fs.rdbuf() << " ]}";
    fs.close();
    json::value jobJson = json::value::parse(jobfile);
    VLOG_2 << "jobJson is: " << jobJson.serialize();
    
    // Now we are going to read the job parameters:
    // [CPU power, Mem Power] (at least for the moment)
    std::ifstream fparam( m_allocPath + JOBPARAM_FILE_PREFIX 
                     + boost::lexical_cast<std::string>(job.jobId)
                     + JOBPARAM_FILE_SUFFIX);
    if ( !fparam ) {
        LOG_FATAL << "[==DC SIMULATOR] Job parameters file "
                  << m_allocPath + JOBPARAM_FILE_PREFIX 
                  << boost::lexical_cast<std::string>(job.jobId)
                  << JOBPARAM_FILE_SUFFIX
                  << " could not be opened. Exiting...";
    }
    std::stringstream paramfile;
    paramfile << "{ \"JobParams\": [" << fparam.rdbuf() << " ]}";
    fparam.close();
    json::value paramJson = json::value::parse(paramfile);
    VLOG_2 << "paramJson is: " << paramJson.serialize();
    
    // Finally, we insert the job in the job map
    if (!parseAllocation(sim, jobJson, paramJson, job)){
        LOG_FATAL << "Error parsing allocation for job " << job.jobId;
    }
    m_jobMap.insert(std::make_pair(job.jobId, job));
    return job.jobId;
}

bool Allocator::parseAllocation(DCSimulator &sim, json::value &jsonData,
                                json::value &jsonParams, Job &job)
{
    try {
        json::array &thisjob = jsonData[U("Allocation")].as_array();
        json::array &thispar = jsonParams[U("JobParams")].as_array();
        // Allocation and Params should have the same size
        // XXX-marina: this is valid for the time being, but may
        //             not apply in the future...
        if (thisjob.size() != thispar.size()){
            LOG_ERROR << "Job allocation and job parameters have different sizes!!";
            return false;
        }
        // Reading allocation info
        for (unsigned int i=0; i< thisjob.size(); i++){
            Nodes node;
            json::array &data = thisjob.at(i).as_array();
            json::array &param = thispar.at(i).as_array();
            node.ircName = data.at(0).as_string();
            node.serverName = data.at(1).as_string();
            sim.getRoom()->findRackName(node.ircName, node.serverName, node.rackName);
            // Rack name is not given in file and needs to be found
            node.numCores = param.at(0).as_integer();
            node.cpuPower = param.at(1).as_double();
            node.memPower = param.at(2).as_double();
            job.nodeVec.push_back(node);
            VLOG_2 << "Allocation --> IRCName= " << job.nodeVec[i].ircName
                   << " -- RackName= " << job.nodeVec[i].rackName
                   << " -- ServerName= " << job.nodeVec[i].serverName
                   << " -- numCores= " << job.nodeVec[i].numCores
                   << " -- cpuPower= " << job.nodeVec[i].cpuPower
                   << " -- memPower= " << job.nodeVec[i].memPower;
        }    
    }
    catch(const json::json_exception &e){
        LOG_ERROR << "Error parsing allocation: " << e.what();
        return false;
    }
    return true;
}

// Changes server allocations (server power values) and removes job
// from queue if it is ended
// - we assume that what we store in job.cpuPwr and job.memPwr is the
//   dynamic power consumption of the server associated to that job
//   (thus, when the job ends we simply subtract that value from the node)
bool Allocator::updateAllocation(DCSimulator &sim, int jobId)
{
    std::map<int, Job>::iterator it = m_jobMap.find(jobId);
    if (it == m_jobMap.end()){
        LOG_ERROR << "Could not find jobId " << jobId << " in job map of size "
                  << m_jobMap.size();
        return false;
    }
    Job &job = it->second;

    // Check begin flag to see if job is starting or ending
    std::function<double(double, double)> changeAlloc;
    if (job.begin){
        VLOG_2 << "Job is starting. Will add power";
        changeAlloc = std::bind(&Allocator::addPower, this,
                                std::placeholders::_1, std::placeholders::_2);
    }
    else {
        VLOG_2 << "Job is ending. Will subtract power";
        changeAlloc = std::bind(&Allocator::subtractPower, this,
                                std::placeholders::_1, std::placeholders::_2);
    }
    
    // Then we do a for through the nodes in the job
    // Here we change values in the DCSimulator "sim" object 
    for (unsigned int i=0; i<job.nodeVec.size(); i++){
        VLOG_1 << "Changing workload of node: rack=" << job.nodeVec[i].rackName
               << " server=" << job.nodeVec[i].serverName ;
        double currCpuPwr=0, currMemPwr=0;
        int currUsedCPUs=0;
        sim.getRoom()->getWorkloadPower(job.nodeVec[i].rackName,
                                        job.nodeVec[i].serverName,
                                        currCpuPwr, currMemPwr, currUsedCPUs);

        double newCpuPwr = changeAlloc(currCpuPwr, job.nodeVec[i].cpuPower);
        double newMemPwr = changeAlloc(currMemPwr, job.nodeVec[i].memPower);
        int newUsedCPUs = (int) changeAlloc( (double) currUsedCPUs,
                                             (double) job.nodeVec[i].numCores);
        
        sim.getRoom()->setWorkloadPower(job.nodeVec[i].rackName,
                                        job.nodeVec[i].serverName,
                                        newCpuPwr, newMemPwr, newUsedCPUs);
        
        
        VLOG_2 << "Current CPUPwr=" << currCpuPwr << " -- Current MemPwr=" << currMemPwr
               << " -- New CPUPwr=" << newCpuPwr << " -- New MemPwr=" << newMemPwr
               << " -- New UsedCPUs= " << newUsedCPUs ;
    }
    
    // If job has ended, we remove it from map
    if (!job.begin){
        m_jobMap.erase(it);
    }
    return true;
}

