/*
 * ===================================================================
 *
 *       Filename:  solana-server.hh
 *    Description:  Solana Server implementation (SPARC T3-2) 
 *
 * ===================================================================
 */

#ifndef SOLANA_SERVER_H
#define SOLANA_SERVER_H

#include "../generic_defs.h"
#include "server.h"

class SolanaServer : public Server
{
public:

    static const int MAX_CPU_TEMP = 87;
    
    static const int IDLE_POWER = 440;
    static const int DEFAULT_FAN_SPEED = 2400;
    static const int MODEL_TINLET = 22;
    static const int MIN_TIN_TOUT_DIFF = 14;

    static const int NUM_CPUS = 2;

    static const int CPU0_IDLE_POWER = 58.3;
    static const int CPU1_IDLE_POWER = 61.6;
    static const int MEM_IDLE_POWER = 320;
    
    SolanaServer();
    virtual ~SolanaServer();

    // Model LUTs
    //XXX-marina: NUM_FAN_SPEED needs to be equal for all servers,
    // and is defined in server.hh
    static constexpr int fanSpeedLUT[NUM_FAN_SPEED] {
        //1800, 2100, 2400, 2700, 3000, 3300, 3600, 3900, 4200
        1800, 2400, 3000, 3600, 4200  
    };

    static constexpr double fanPowerLUT[NUM_FAN_SPEED] {
        //9.32, 11.86, 14.73, 18.43, 22.34, 27.72, 33.39, 38.69, 46.33
        9.32, 14.73, 22.34, 33.39, 46.33
            
    };

    static constexpr double tempCpu0LUT[NUM_FAN_SPEED][2] {
        {58.1, 0.5718}, {50.5, 0.4063},
        {46.9, 0.3115}, 
        {45.4, 0.2708}, {44.2, 0.2472}
    };

    static constexpr double tempCpu1LUT[NUM_FAN_SPEED][2] {
        {56.5, 0.5120}, {48.4, 0.3618},
        {45.3, 0.2645}, 
        {44.1, 0.2233}, {42.4, 0.1997}
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
    virtual void recomputeFS(std::string policy);
    virtual void setFanSpeedByIdx(int idx);
    
private:
    int getIndex(int fanSpeed);
    void perCpuDynPower(double power, std::vector<double> &perCpuPwr);
};


#endif /* SOLANA-SERVER_H */

