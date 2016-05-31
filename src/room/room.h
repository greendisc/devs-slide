/*
 * ===================================================================
 *
 *       Filename:  room.hh
 *    Description:  Room base class (creates and fills room) 
 *
 * ===================================================================
 */

#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <map>
#include <vector>

//#include <boost/lexical_cast.hpp>
#include "../generic_defs.h"
#include "rack-irc.h"

class Room
{
public:

    typedef struct{
        double serverITPower;
        double serverFSPower;
        double ircPower;
        double airflow;
        double avgTout;
    }RackParams; //Parameters for the couple of racks within an IRC
    
    typedef struct{
        double serverITPower;
        double serverFSPower;
        double ircPower;
        std::string roomException;
        std::string msg;
    } RoomPower;

    //Exceptions
    static constexpr char MAX_TEMP_EX[] = "MaxTemp";
    static constexpr char PERF_EX[] = "Perf";
    
    Room();
    virtual ~Room();

    // Getters and setters
    RoomPower& getRoomPowerSummary();
    
    // Virtual functions (for someone else to implement)
    virtual void setCoupleInletTemp(const std::string &name, int temp) = 0;
    virtual void setFanSpeed(const std::string &rackName, const std::string &serverName,
                             int fanSpeed) = 0;
    virtual void setWorkloadPower(const std::string &rackName, const std::string &serverName,
                                  double cpuPower, double memPower, int usedCPUs) = 0;
    virtual void getWorkloadPower(const std::string &rackName, const std::string &serverName,
                                  double &cpuPower, double &memPower, int &usedCPUs) = 0;

    virtual void getWorkloadAlloc(std::vector<Nodes> &alloc) = 0;
    
    virtual bool findRackName(const std::string &ircName, const std::string &serverName,
                              std::string &rackName) = 0;
    
    virtual CoolingParams* computePower(double tempWaterCold) = 0;
virtual RackParams* computeRackPower(RackAndIRC *rackIrc, int fanSpeedIdx) = 0;
    
    virtual int recomputeFS (std::string policy) = 0;
    
    virtual void checkExceptions() = 0 ;
    // Get the heat generated in each IRC
    virtual void getIRCDynPower(std::map<std::string,double> &dynWkload) = 0;
    virtual double getAvgTinlet() = 0;
    
    RackAndIRC* getRackIrcCoupleByName(const std::string &name);
    void clearExceptions();
    void addException(std::string &ex, std::string &msg);

    
protected:
    std::map<std::string, RackAndIRC*> m_coupled;
    RoomPower m_powerSummary;   
};

#endif /* ROOM_H */

