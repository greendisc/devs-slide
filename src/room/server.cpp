/*
 * ===================================================================
 *
 *       Filename:  server.cc
 *    Description:  Server base class 
 *
 * ===================================================================
 */

#include "server.h"

constexpr char Server::SERVER_OPTIMUM_POLICY[];

Server::Server(double idlePower, int fanSpeed)
    : m_tinlet(DEFAULT_INLET_TEMPERATURE),
      m_fanSpeed(fanSpeed),
      m_cpuDynPower(0.0), m_memPower(0.0), m_usedCPUs(0),
      m_totalHeatPower(idlePower), m_fanSpeedPower(0.0)
{
}
Server::~Server()
{
}

// Implementing setters
void Server::setInletTemp(int tinlet)
{
    m_tinlet = tinlet;
}

void Server::setFanSpeed(int fanSpeed)
{
    m_fanSpeed = fanSpeed;
}

void Server::setCpuDynPower(double power)
{
    m_cpuDynPower = power;
}

void Server::setMemPower(double power)
{
    m_memPower = power;
}

void Server::setUsedCPUs(int cpus)
{
    m_usedCPUs = cpus;
}

// Implementing getters
double Server::getTotalHeatPower()
{
    return m_totalHeatPower;
}

std::vector<double>& Server::getCPUTemperature()
{
    return m_cpuTemp;
}

double Server::getFanSpeedPower()
{
    return m_fanSpeedPower;
}

double Server::getAirflow()
{
    return computeAirflow();
}

int Server::getFanSpeed()
{
    return m_fanSpeed;
}

double Server::getCpuDynPower()
{
    return m_cpuDynPower ;
}

double Server::getMemPower()
{
    return m_memPower ;
}

int Server::getUsedCPUs()
{
    return m_usedCPUs ;
}

