/*
 * ===================================================================
 *
 *       Filename:  apc-irc.cc
 *    Description:  In-row cooler APC implementation 
 *
 * ===================================================================
 */

#include "apc-irc.h"

#include <math.h>

//Adding constexpr definitions so that linker does not complain
constexpr int ApcIrc::airflowLUT[NUM_AIRFLOW_VALUES];
constexpr int ApcIrc::gallonsLUT[NUM_GALLONS_SETTING];
constexpr double ApcIrc::perfLUT[NUM_AIRFLOW_VALUES][NUM_GALLONS_SETTING];

ApcIrc::ApcIrc()
    : IRC()
{
    VLOG_2 << "APC IRC created";
}

ApcIrc::~ApcIrc()
{
}

int ApcIrc::getAirflowIdx (double airflow)
{
    for (int i=0; i<NUM_AIRFLOW_VALUES; i++){
        if (airflowLUT[i]>airflow){
            return i;
        }
    }
    return -1;
}
double ApcIrc::computePower(double airflow)
{
    int idx = getAirflowIdx(airflow);
    double realflow = airflowLUT[idx];

    double power = 0.000000015658*pow(realflow,3)+0.000030196*pow(realflow,2)
        -0.0317*realflow+198.9612;
    
    return power;
}

double ApcIrc::computeGallons(double airflow, double heatPower, double tempAirHot,
                              double tempWaterCold)
{
    int idx = getAirflowIdx(airflow);
    m_tempWaterCold = tempWaterCold;
    double perf = heatPower /(tempAirHot - tempWaterCold);
    VLOG_2 << " -- IRC heatPower= " << heatPower << " tempAirHot= " << tempAirHot
           << " tempWaterCold= " << m_tempWaterCold;
    for (int i=0; i<NUM_GALLONS_SETTING; i++){
        if (perfLUT[idx][i] > perf){
            VLOG_2 << " -- IRC performance= " << perf << " gallons= " << gallonsLUT[i];
            return gallonsLUT[i];
        }
    }
    LOG_WARNING << "Performance " << perf << " cannot be met with airflow " << airflow;
    return -1;
}

double ApcIrc::computeHotWaterTemp(double waterFlow, double heatPower)
{
    double delta = heatPower/(waterFlow*GPM_CONVERSION*RHO_WATER*CP_WATER*1000);
    VLOG_2 << " -- Hot water temperature is: " << m_tempWaterCold + delta
           << " (heat Power= " << heatPower << ", waterFlow=" << waterFlow;
    return (m_tempWaterCold + delta);
}

int ApcIrc::getGallons(int idx)
{
    if ((idx > 0) && (idx < NUM_GALLONS_SETTING)){
        return (gallonsLUT[idx]);
    }
    else {
        return -1;
    }
}

