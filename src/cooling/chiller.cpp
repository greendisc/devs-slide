/*
 * ========================================================================
 *
 *       Filename:  chiller.cc
 *    Description:  Overall-DC power consumption simulator 
 *
 * =======================================================================
 */
#include "chiller.h"

#include <string>
#include <vector>
#include <list>


Chiller::Chiller(const ChillerParams &params)
{
    m_params = params;
    m_chillerPower = 0;
    m_towerPower = 0;
    m_electricPower = 0;
}

Chiller::~Chiller()
{
}

//Getters and setters
double Chiller::getPower()
{
    return m_electricPower;
}

double Chiller::getChillerPower()
{
    return m_chillerPower;
}

double Chiller::getTowerPower()
{
    return m_towerPower;
}

void Chiller::setOutdoorTemp (double temp)
{
    m_params.outdoorTemp = temp;
}

void Chiller::setWaterColdTemp(double temp)
{
    m_params.coldWaterTemp = temp;
}

double Chiller::getWaterColdTemp()
{
    return m_params.coldWaterTemp;
}

