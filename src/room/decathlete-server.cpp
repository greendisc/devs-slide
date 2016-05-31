/*
 * ===================================================================
 *
 *       Filename:  decathlete-server.cc
 *    Description:  Decathlete OCP server implementation (Intel SandyBridge) 
 *
 * ===================================================================
 */

#include "decathlete-server.h"

//Declaring constexpr so that linker does not complain:
constexpr int DecathleteServer::fanSpeedLUT[NUM_FAN_SPEED];
constexpr double DecathleteServer::fanPowerLUT[NUM_FAN_SPEED];
constexpr double DecathleteServer::tempCpu0LUT[NUM_FAN_SPEED][2];
constexpr double DecathleteServer::tempCpu1LUT[NUM_FAN_SPEED][2];

DecathleteServer::DecathleteServer()
    : Server(IDLE_POWER, DEFAULT_FAN_SPEED)
{    
    VLOG_2 << "Decathlete server created";
    computeFanPower();
}

DecathleteServer::~DecathleteServer()
{    
}

// Power computation: function responsible for filling-in
// all power data values
double DecathleteServer::computePower()
{
    // Assume workload power has already been filled-in
    // during workload parsing stage
    VLOG_2 << "Computing Decathlete Server power";
    computeCPUTemp(m_cpuDynPower);
    double leakage = computeLeakage();
    m_totalHeatPower = IDLE_POWER + m_memPower + m_cpuDynPower + leakage;

    VLOG_2 << " -- Total heat power (total power - fan power) " << m_totalHeatPower;     
    return m_totalHeatPower;
}

void DecathleteServer::computeCPUTemp(double power)
{
    int idx = getIndex(m_fanSpeed);
    if (idx < 0){
        LOG_ERROR << "Fan speed not found!";
    }
    
    // Setting CPU(0,1) temperature
    m_cpuTemp.clear();
    std::vector<double> dynPwr;
    perCpuDynPower(m_cpuDynPower, dynPwr);
    double temp = (m_tinlet - MODEL_TINLET) + tempCpu0LUT[idx][0]+tempCpu0LUT[idx][1]*dynPwr[0];
    m_cpuTemp.push_back(temp);

    temp = (m_tinlet - MODEL_TINLET) + tempCpu1LUT[idx][0]+tempCpu1LUT[idx][1]*dynPwr[1];
    m_cpuTemp.push_back(temp);

    VLOG_2 << " -- Temperature is: CPU0= " << m_cpuTemp[0] << " CPU1= " << m_cpuTemp[1];
    
}

double DecathleteServer::computeLeakage()
{
    // Extracted with samples between 50 and 72C:
    // 27.5-1.1016*a+0.0112*a.^2
    double leak0, leak1;
    leak0 = 0.0112*(m_cpuTemp[0]*m_cpuTemp[0]) - 1.016*m_cpuTemp[0] + 27.5;
    leak1 = 0.0112*(m_cpuTemp[1]*m_cpuTemp[1]) - 1.016*m_cpuTemp[1] + 27.5;

    VLOG_2 << " -- Leakage is: CPU0= " << leak0 << " CPU1=" << leak1
           << " total=" << leak0+leak1;
    return (leak0+leak1);
}

// I am assuming same airflow than Solana
double DecathleteServer::computeAirflow()
{
    double airflow = (0.0099*m_fanSpeed-13.4612)/2;

    VLOG_2 << " -- Fan speed is: " << m_fanSpeed
           << " Airflow is: " << airflow;
    return airflow;
}

double DecathleteServer::computeFanPower()
{
    int idx = getIndex(m_fanSpeed);
    if (idx < 0){
        LOG_ERROR << "Fan speed not found!";
        return 0.0;
    }
    // Setting fan speed
    m_fanSpeedPower = fanPowerLUT[idx];
    VLOG_2 << " -- Fan power " << m_fanSpeedPower;
    return m_fanSpeedPower;
}

double DecathleteServer::computeServerTout( double airflow )
{
    double tout = m_tinlet + m_totalHeatPower/(airflow*CP_AIR*RHO_AIR*1000/CFM_CONVERSION);
    if ((tout - m_tinlet) < MIN_TIN_TOUT_DIFF ){
        VLOG_2 << " -- Tout= " << m_tinlet + MIN_TIN_TOUT_DIFF << " (forced to minimum, heat= "
               << m_totalHeatPower <<" W , airflow= " << airflow <<")";
        return (m_tinlet + MIN_TIN_TOUT_DIFF);
    }
    VLOG_2 << " -- Tout= " << tout;
    return tout;
}

int DecathleteServer::getIndex(int fanSpeed)
{
    for (int i=0; i<NUM_FAN_SPEED; i++){
        if ( fanSpeedLUT[i] == fanSpeed){
            return i;
        }
    }
    return -1;
}

void DecathleteServer::perCpuDynPower(double power, std::vector<double> &perCpuPwr)
{
    double pwr = power/NUM_CPUS;
    perCpuPwr.clear();
    for (int i=0; i< NUM_CPUS; i++){
        perCpuPwr.push_back(pwr);
    }
}

bool DecathleteServer::checkMaxCPUTemp()
{
    for (unsigned int i=0; i<m_cpuTemp.size(); i++){
        if (m_cpuTemp[i] >= MAX_CPU_TEMP){
            VLOG_2 << "CPU temperature too high: " << m_cpuTemp[i] ;
            return true;
        }
    }
    return false;
}

void DecathleteServer::setFanSpeedByIdx (int idx)
{
    m_fanSpeed = fanSpeedLUT[idx];
}

void DecathleteServer::recomputeFS(std::string policy)
{
    //Optimum proactive fan control policy
    if (policy.compare(SERVER_OPTIMUM_POLICY) == 0){
        double fanPlusLeak=10000;
        for ( int i=0; i< NUM_FAN_SPEED; i++ ){
            setFanSpeed(fanSpeedLUT[i]);
            computeCPUTemp(m_cpuDynPower);
            double leak = computeLeakage();
            double fanpower = computeFanPower();
            if ( (leak+fanpower) < fanPlusLeak ){
                fanPlusLeak = leak+fanpower;
                VLOG_2 << " -- Fan+Leak power for FS: " << getFanSpeed()
                       << " is: " << fanPlusLeak ;
            }
            else {
                VLOG_2 << " -- FanSpeed: " << getFanSpeed()
                       << " increases power (" << leak+fanpower << ")";
                setFanSpeed(fanSpeedLUT[i-1]);
                break;
            }
        }
    }
}

double DecathleteServer::getIdlePower()
{
    return (double) IDLE_POWER;
}
