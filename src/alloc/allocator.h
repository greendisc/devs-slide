/*
 * ===================================================================
 *
 *       Filename:  allocator.hh
 *    Description:  Allocator simulator 
 *
 * ===================================================================
 */
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <iostream>
#include <vector>
#include <map>
#include <functional>

#include "cpprest/basic_types.h"
#include <sys/time.h>

#include "../dc-simulator.h"
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;
using namespace web::json;

class DCSimulator;

class Allocator
{
public:
    Allocator(time_t baseTime, time_t firstSubmit, std::string &allocPath, bool offline);
    virtual ~Allocator();

    static constexpr char ALLOCOUT_FILE_PATH[] = "/tmp/";
    static constexpr char ALLOCOUT_FILE_PREFIX[] = "alloc_out_";
    static constexpr char ALLOCOUT_FILE_SUFFIX[] = ".txt";

    static constexpr char JOBPARAM_FILE_PREFIX[] = "param_job_";
    static constexpr char JOBPARAM_FILE_SUFFIX[] = ".txt";
    
    static constexpr char CURRENT_ALLOC_FILE[] = "/tmp/prev_alloc.txt";
    
    // Attributes

    typedef struct {
        bool begin;
        int jobId;      //! Job id (starts on 1001 for slurm-sim) 
        time_t startTime; 
        time_t endTime;
        int numProcs;   //! Number of processors allocated
        std::vector<Nodes> nodeVec;
    }Job;
    
    // Methods
    void printCurrentAlloc(DCSimulator &sim);
    int  parseNewJob(DCSimulator &sim, std::string &buffer);
    bool parseAllocation(DCSimulator &sim,
                         json::value &jsonData,
                         json::value &jsonParams, Job &job);
    bool updateAllocation(DCSimulator &sim, int jobId);
    std::map<int, Job>& getJobMap();
    void setCurrentTime(time_t time);
    time_t getCurrentTime();
    
private:
    //Attributes
    std::map<int, Job> m_jobMap;
    time_t currentTime;
    time_t m_baseTime;
    time_t m_firstSubmit;
    time_t m_lastRoundTime;
    std::string m_allocPath;
    bool offline;
    bool firstRound;
    
    //Methods
    double addPower(double op1, double op2);
    double subtractPower(double op1, double op2);

};

#endif /* _ALLOCATOR_H_ */
