/*
 * ===================================================================
 *
 *       Filename:  decathlete-server.hh
 *    Description:  Decathlete OCP server implementation (Intel SandyBridge) 
 *
 * ===================================================================
 */

#ifndef DECATHLETE_SERVER_H
#define DECATHLETE_SERVER_H

#include "../generic_defs.h"
#include "server.h"

class DecathleteServer : public Server
{
public:

    static const int MAX_CPU_TEMP = 85;
    
    static const int DEFAULT_FAN_SPEED = 7000;
    static const int MODEL_TINLET = 22;
    static const int MIN_TIN_TOUT_DIFF = 5;

    static const int NUM_CPUS = 2;

    static const int CPU0_IDLE_POWER = 50;
    static const int CPU1_IDLE_POWER = 50;
    static const int MEM_IDLE_POWER = 4;
    static const int DISK_IDLE_POWER = 16;
    
    static const int IDLE_POWER = CPU0_IDLE_POWER
        + CPU1_IDLE_POWER + MEM_IDLE_POWER + DISK_IDLE_POWER ;
    
    DecathleteServer();
    virtual ~DecathleteServer();

    // Model LUTs
    static constexpr int fanSpeedLUT[NUM_FAN_SPEED] {
        6000, 7000, 8000, 9000, 10000 
    };

    static constexpr double fanPowerLUT[NUM_FAN_SPEED] {
        14.4, 20.0, 27.2, 36.4, 48.0  //Real decathlete model
    };

    static constexpr double tempCpu0LUT[NUM_FAN_SPEED][2] {
        {44.3, 0.7353}, {47, 0.5156},
        {45.3, 0.3957}, {44, 0.3478}, {44, 0.3111}
    };

    static constexpr double tempCpu1LUT[NUM_FAN_SPEED][2] {
        {44.3, 0.7353}, {47, 0.5156},
        {45.3, 0.3957}, {44, 0.3478}, {44, 0.3111}
    };
    
    // Power computation: function responsible for filling-in
    // all power data values
    virtual double computePower();
    virtual void computeCPUTemp(double power);
    virtual double computeLeakage();
    virtual double computeAirflow();
    virtual double computeFanPower();
    virtual double computeServerTout(double airflow);
    
    virtual bool checkMaxCPUTemp();

    virtual double getIdlePower();
    virtual void setFanSpeedByIdx(int idx);
    virtual void recomputeFS(std::string policy);
    
private:
    int getIndex(int fanSpeed);
    void perCpuDynPower(double power, std::vector<double> &perCpuPwr);
};

#endif /* DECATHLETE_SERVER_H */
