#ifndef GENERIC_DEFS_H
#define GENERIC_DEFS_H

#include <iostream>

/**
   some debug helper macros
*/

//If NDEBUG is defined, them there are no DEBUG_LOG messages
#define  NDEBUG 1
//Customized log messages
#define     LOG_INFO        std::cout << "[INFO] [" << __FUNCTION__<< "] "
#define     LOG_WARNING     std::cout << "[WARNING] [" << __FUNCTION__<< "] "
#define     LOG_ERROR       std::cout << "[ERROR] [" << __FUNCTION__<< "] "
#define     LOG_FATAL       std::cout << "[FATAL] [" << __FUNCTION__<< "] "

#define     VLOG_1          std::cout << "[Verbose1] [" << __FUNCTION__<< "] "
#define     VLOG_2          std::cout << "[Verbose2] [" << __FUNCTION__<< "] "

#define     DEBUG_LOG       std::cout << "[Debug] [" << __FUNCTION__<< "] "

typedef struct{
    double waterFlow;
    double waterTemp;
    bool freeCooling;
    bool converged;
} CoolingParams;

// Struct will need to change in the future to incorporate workload data
// (i.e. slope prediction and other stuff)
typedef struct{
    std::string ircName;
    std::string rackName;
    std::string serverName;
    int numCores;
    double cpuPower; //contribution to power of this job to the node
    double memPower;
}Nodes;


// Constant definition
#define    CP_AIR       1.006      // (KJ/KgC)
#define    RHO_AIR      1.1644     // (kg/m3)

#define    CP_WATER     4.187      // (KJ/KgC)
#define    RHO_WATER    999.3      // (kg/m3)

#define    CFM_CONVERSION          2118.88
#define    GPM_CONVERSION          0.00006389
#define    KW_TON_CONVERSION       0.000284345
#define    HP_TO_WATT_CONVERSION   745.7


#endif /* GENERIC_DEFS_H */
