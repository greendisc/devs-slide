/*
 * ===================================================================
 *
 *       Filename:  pump.cc
 *    Description:  Pump base class for power consumption simulator 
 *
 * ===================================================================
 */

#include "pump.h"

Pump::Pump()
{
    
}

Pump::~Pump()
{
    
}

//Getters and setters
double Pump::getPower()
{
    return m_electricPower;
}

