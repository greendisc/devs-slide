/*
 * ===================================================================
 *
 *       Filename:  mghpcc-room.hh
 *    Description:  Mass. Green HPC Center room 
 *
 * ===================================================================
 */

#ifndef MGHPCC_POD_H
#define MGHPCC_POD_H

#include <iostream>

#include <iostream>
#include <vector>
#include "room.h"

class MGHPCCPod : public Room
{
public:
    
    MGHPCCPod(const std::string &IRCModel,
                  std::vector<RackAndIRC::RackAndIRCParams> &rackIRC);
    virtual ~MGHPCCPod();

    static const int NUM_COUPLES = 10;
    static const int NUM_RACKS_PER_IRC = 2;

    virtual void setCoupleInletTemp(const std::string &name, int temp);
    virtual void setFanSpeed(const std::string &rackName, const std::string &serverName,
                                 int fanSpeed);
    virtual void setWorkloadPower(const std::string &rackName, const std::string &serverName,
                                  double cpuPower, double memPower, int usedCPUs);

    virtual void getWorkloadPower(const std::string &rackName, const std::string &serverName,
                                  double &cpuPower, double &memPower, int &usedCPUs);

    virtual void getWorkloadAlloc(std::vector<Nodes> &alloc);
    
    virtual Server* findServer(const std::string &rackName, const std::string &serverName);
    virtual bool findRackName(const std::string &ircName, const std::string &serverName,
                              std::string &rackName);
    
    // Power consumption calculation
    virtual CoolingParams* computePower(double tempWaterCold);

    // Computing a certain fan speed policy
    virtual int recomputeFS (std::string policy);
    
    // Check if any value implies an exception
    virtual void checkExceptions();

    // Get the heat generated in each IRC
    virtual void getIRCDynPower(std::map<std::string,double> &dynWkload);
    virtual double getAvgTinlet();
    
    //Compute power consumed in each IRC
    virtual Room::RackParams* computeRackPower(RackAndIRC *rackIrc, int fanSpeedIdx);
    
protected:
    
};

#endif /* MGHPCC_POD_H */
