/*
 * ===================================================================
 *
 *       Filename:  plantB-chiller.cc
 *    Description:  PlantB Chiller model  
 *
 * ===================================================================
 */

#include "plantB-chiller.h"

constexpr double PlantBChiller::waterColdLUT[NUM_WATERCOLD_TEMPS];
constexpr double PlantBChiller::perfLUT[NUM_WATERCOLD_TEMPS];

PlantBChiller::PlantBChiller( const ChillerParams &params )
    : Chiller(params)
{
    LOG_INFO << "PlantBChiller created";
}

PlantBChiller::~PlantBChiller()
{
}

//Power models
double PlantBChiller::computePower(CoolingParams *params)
{
    params->freeCooling=false;
    double heat = params->waterFlow*GPM_CONVERSION*RHO_WATER*CP_WATER*1000
        *(params->waterTemp-m_params.coldWaterTemp);
    double tons = heat*KW_TON_CONVERSION;
    
    double eff = getEfficiency(m_params.coldWaterTemp);
    if (eff < 0){
        LOG_ERROR << "Invalid value for chiller efficiency";
    }
    m_electricPower = tons*1000*eff;
    VLOG_2 << " -- Chiller: heat= " << heat << " eff= " << eff;
    VLOG_2 << "Chiller electrical power consumption is " << m_electricPower;
    m_chillerPower = m_electricPower;
    return m_electricPower;
}

double PlantBChiller::getEfficiency(double tempColdWater)
{
    for (int i=0 ; i<NUM_WATERCOLD_TEMPS; i++){
        if (tempColdWater <= waterColdLUT[i] ){
            return perfLUT[i];
        }
    }
    return -1;
}

bool PlantBChiller::reduceWaterTemp()
{
    return false;
}

double PlantBChiller::getMaxApproach(double tempOut)
{
    return 0.0;
}

double PlantBChiller::assertApproach(double tempOut)
{
    return 0.0;
}

double PlantBChiller::getFreeCoolingWaterTemp()
{
    return 0.0;
}

double PlantBChiller::getChillerWaterTemp()
{
    return 0.0;
}

double PlantBChiller::getTevapForMaxCOP(double tempOut, double load)
{
    return 0.0;
}

double PlantBChiller::getTowerRange()
{
    return 0.0;
}

