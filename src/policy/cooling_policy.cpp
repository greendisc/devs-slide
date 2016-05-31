/*
 * ===================================================================
 *
 *       Filename:  cooling_policy.cc
 *    Description:  Cooling policy main class
 *
 * ===================================================================
 */

#include "cooling_policy.h"

CoolingPolicy::CoolingPolicy(DCSimulator &sim)
    : sim(sim)
{
    LOG_INFO << "Cooling policy created";
}

CoolingPolicy::~CoolingPolicy()
{
}

void CoolingPolicy::run( double tempOut )
{
    
}
