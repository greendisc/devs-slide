/*
 * ===================================================================
 *
 *       Filename:  ASHRAE-pump.cc
 *    Description:  Water pump implementation according to ASHRAE  
 *
 * ===================================================================
 */
#include "ashrae-pump.h"

// constexpr double ASHRAEPump::PUMP_DELTA_PRESSURE;
// constexpr double ASHRAEPump::PUMP_FACTOR;
// constexpr double ASHRAEPump::PUMP_EFFICIENCY; 

ASHRAEPump::ASHRAEPump()
    : Pump()
{
    LOG_INFO << "ASHRAE pump created";
}

ASHRAEPump::~ASHRAEPump()
{    
}

//Power models
double ASHRAEPump::computePower(CoolingParams *params)
{
    double hydraPower = params->waterFlow*PUMP_DELTA_PRESSURE*PUMP_FACTOR;
    m_electricPower = hydraPower/PUMP_EFFICIENCY;
    VLOG_2 << "ASHRAE Pump Electrical power " << m_electricPower;
    return m_electricPower;
}

