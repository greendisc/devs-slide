/*
 * ===================================================================
 *
 *       Filename:  fixed_inlet.cc
 *    Description:  Fixed inlet temperature cooling policy main class
 *
 * ===================================================================
 */

#include "fixed_inlet.h"

FixedInletPolicy::FixedInletPolicy(DCSimulator &sim)
    : CoolingPolicy(sim)
{
    LOG_INFO << "FixedInlet cooling control policy working";
}

FixedInletPolicy::~FixedInletPolicy()
{
}

void FixedInletPolicy::run( double tempOut )
{
    VLOG_1 << "Fixed inlet policy changes nothing";
}


