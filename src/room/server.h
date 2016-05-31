/*
 * ===================================================================
 *
 *       Filename:  server.hh
 *    Description:  Server base class 
 *
 * ===================================================================
 */
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include "../generic_defs.h"

class Server
{
public:

    static const int DEFAULT_INLET_TEMPERATURE = 18;
    static constexpr char SERVER_OPTIMUM_POLICY[] = "server-proactive";
    static const int NUM_FAN_SPEED = 5; // low, med-low, med, med-high, high
    
    Server(double idlePower, int fanSpeed);
    virtual ~Server();

    // Getters and setters
    double getTotalHeatPower();
    std::vector<double>& getCPUTemperature();
    double getFanSpeedPower();
    double getAirflow();
    int getFanSpeed();
    double getCpuDynPower();
    double getMemPower();
    int getUsedCPUs();
    virtual double getIdlePower() = 0;
    
    void setInletTemp(int tinlet);
    void setFanSpeed(int fanSpeed);
    virtual void setFanSpeedByIdx(int idx)=0;
        
    // FIXME-marina: for the moment, we consider that input consists on
    // CPU and memory dynamic power values... (instead of a workload measure)
    void setCpuDynPower(double power);
    void setMemPower(double power);
    void setUsedCPUs(int cpus);

    // Model computation
    virtual double computePower() = 0;
    virtual void computeCPUTemp(double power) = 0;
    virtual double computeLeakage() = 0;
    virtual double computeAirflow() = 0;
    virtual double computeFanPower() = 0;
    virtual double computeServerTout( double airflow) = 0;

    virtual bool checkMaxCPUTemp() = 0;

    virtual void recomputeFS(std::string policy) = 0;
    
protected:

    //Given magnitudes:
    int m_tinlet;
    int m_fanSpeed;
    double m_cpuDynPower;
    double m_memPower;
    int m_usedCPUs;
    
    //Model-dependent magnitudes
    std::vector<double> m_cpuTemp; //vector because we may have several CPUs
    double m_totalHeatPower;
    double m_fanSpeedPower;
    
};



#endif /* SERVER_H */
