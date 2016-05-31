/*
 * ===================================================================
 *
 *       Filename:  mghpcc-room.cc
 *    Description:  Room base class (creates and fills room)   
 *
 * ===================================================================
 */

#include "mghpcc-pod.h"
#include <string>
#include <iostream>

MGHPCCPod::MGHPCCPod(const std::string &IRCModel,
                     std::vector<RackAndIRC::RackAndIRCParams> &rackIRC)
{
    LOG_INFO << "Mass Green HPC Center POD created with IRC of type " << IRCModel;
    
    for (unsigned int i=0; i<rackIRC.size(); i++){
        m_coupled.insert(std::make_pair(rackIRC[i].coupleName,
                                        new RackAndIRC(rackIRC[i], IRCModel)));
    }
}

MGHPCCPod::~MGHPCCPod()
{
}

void MGHPCCPod::setCoupleInletTemp(const std::string &name, int temp)
{
    RackAndIRC* couple = m_coupled[name];

    //Setting temperature of in-row cooler
    couple->getIRC()->setInletTemp(temp);

    //Setting inlet temperature of all servers
    std::map<std::string,Rack>::iterator it;
    for (it=couple->m_rack.begin(); it!=couple->m_rack.end();++it){
        Rack &rack = it->second;
        std::map<std::string,Server*>::iterator it2;
        for (it2=rack.m_servers.begin(); it2!=rack.m_servers.end(); ++it2){
            Server* server = it2->second;
            server->setInletTemp(temp);
            VLOG_2 << "Setting inlet temperature " << temp
                   << " to server " << it2->first << " in rack " << it->first ;
        }
    }
}

void MGHPCCPod::setFanSpeed(const std::string &rackName, const std::string &serverName,
                            int fanSpeed)
{
    Server *server = findServer(rackName, serverName);
    if(!server){
        LOG_ERROR << "Server is null.";
        return;
    }
    server->setFanSpeed(fanSpeed);
    VLOG_2 << "Setting fan speed " << fanSpeed << " of server " << serverName
           << " of rack " << rackName ;
}

void MGHPCCPod::setWorkloadPower(const std::string &rackName, const std::string &serverName,
                                 double cpuPower, double memPower, int usedCPUs)
{
    Server *server = findServer(rackName, serverName);
    if(!server){
        LOG_ERROR << "Server is null.";
        return;
    }
    server->setCpuDynPower(cpuPower);
    server->setMemPower(memPower);
    server->setUsedCPUs(usedCPUs);
    VLOG_2 << "Setting [CPUpower= " << cpuPower << ", memory Power= " << memPower
           << "] of server " << serverName << " of rack " << rackName ;
}

void MGHPCCPod::getWorkloadPower(const std::string &rackName, const std::string &serverName,
                                 double &cpuPower, double &memPower, int &usedCPUs)
{
    Server *server = findServer(rackName, serverName);
    if(!server){
        LOG_ERROR << "Server is null.";
        return;
    }
    cpuPower = server->getCpuDynPower();
    memPower = server->getMemPower();
    usedCPUs = server->getUsedCPUs();
    VLOG_2 << "Getting [CPUpower= " << cpuPower << ", memory Power= " << memPower
           << "], CPUs used=" << usedCPUs << " of server " << serverName
           << " of rack " << rackName ;
}

bool MGHPCCPod::findRackName(const std::string &ircName, const std::string &serverName,
                             std::string &rackName)
{
    //Select Irc couple
    if (m_coupled.find(ircName) == m_coupled.end()){
        LOG_WARNING << "IRC named " << ircName << " not found";
        return false;
    }
    else{
        RackAndIRC* rackIrc = m_coupled[ircName];
        std::map<std::string, Rack>::iterator it;
        for (it=rackIrc->m_rack.begin(); it!=rackIrc->m_rack.end(); ++it){
            std::string thisRackName = it->first;
            Rack &rack = it->second;
            VLOG_2 << "Searching for server " << serverName << " in rack "
                   << thisRackName;

            //Check if server is here
            std::map<std::string, Server*>::iterator it2;
            if (rack.m_servers.find(serverName) != rack.m_servers.end()){
                rackName = thisRackName;
                VLOG_2 << "Found server " << serverName << " in rack " << rackName ;
                return true;
            }
        }
    }
    return false;
}

Server* MGHPCCPod::findServer(const std::string &rackName, const std::string &serverName)
{
    std::map<std::string, RackAndIRC*>::iterator it;
    //FIXME-marina: Find rack (I should have made this more efficient...)
    for( it=m_coupled.begin();it!=m_coupled.end();++it ){
        RackAndIRC* rackIrc = it->second;
        if (rackIrc->m_rack.find(rackName) != rackIrc->m_rack.end()){
            //Rack is in this Rack-IRC couple
            Server *server = rackIrc->m_rack[rackName].m_servers[serverName];
            return server;
        }
    }
    LOG_WARNING << "Rack or server not found... Returning NULL pointer";
    return NULL;
}

CoolingParams* MGHPCCPod::computePower(double tempWaterCold)
{
    //Clear previous exceptions
    clearExceptions();
    
    //Compute power for each Rack-IRC pair
    std::map<std::string, RackAndIRC*>::iterator it;
    double totalGallons = 0;
    double accumWaterHeat = 0;
    m_powerSummary.serverITPower = 0;
    m_powerSummary.serverFSPower = 0;
    m_powerSummary.ircPower = 0;
    
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        VLOG_2 << "Computing power for Rack-IRC couple " << it->first;

        // Compute Rack parameters struct
        RackParams *rpars = computeRackPower(it->second, -1);
       
        //Compute IRC power, amount of gallons needed and temperature of hot water
        VLOG_2 << "Computing IRC parameters" ;
        double gallons = it->second->getIRC()->
            computeGallons(rpars->airflow, rpars->serverITPower,
            rpars->avgTout, tempWaterCold);

        if (gallons < 0){
            std::string ex = std::string(PERF_EX);
            std::string msg = "";
            addException(ex, msg);
            return NULL;
        }
        double hotWaterTemp = it->second->getIRC()->computeHotWaterTemp(gallons,
                                                                        rpars->serverITPower);
        accumWaterHeat += gallons*hotWaterTemp;
        totalGallons+=gallons;

        // Save power values
        m_powerSummary.serverITPower += rpars->serverITPower;
        m_powerSummary.serverFSPower += rpars->serverFSPower;
        m_powerSummary.ircPower += rpars->ircPower;
    }

    CoolingParams *cooling = new CoolingParams{};
    cooling->waterFlow=totalGallons;
    cooling->waterTemp=accumWaterHeat/totalGallons;
    VLOG_1 << "Cooling params: waterFlow=" << cooling->waterFlow
           << " waterHotTemp=" << cooling->waterTemp ;
    
    return cooling;
}

Room::RackParams* MGHPCCPod::computeRackPower(RackAndIRC *rackIrc, int fanSpeedIdx)
{
    Room::RackParams *params = new RackParams();

    double accumToutHeat = 0;
    double accumAirflow = 0;
    
    //Compute per-rack power values
    std::map<std::string, Rack>::iterator it2;
    for (it2=rackIrc->m_rack.begin(); it2!=rackIrc->m_rack.end(); ++it2){
        VLOG_2 << "Computing power for rack " << it2->first;
        Rack &rack = it2->second;
        std::map<std::string, Server*>::iterator it3;
        for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
            VLOG_2 << "Computing power for server " << it3->first;
            Server *server = it3->second;
            params->serverITPower += server->computePower();
            if (fanSpeedIdx>=0){
                VLOG_2 << "Setting fan speed before computing power";
                server->setFanSpeedByIdx(fanSpeedIdx);
            }
            server->computeFanPower();
            params->serverFSPower += server->getFanSpeedPower();
            
            double airflow = server->computeAirflow();
            double tout = server->computeServerTout(airflow);
            accumToutHeat+=tout*airflow;
            accumAirflow+=airflow;
        }
    }
    params->airflow = accumAirflow;
    params->avgTout = accumToutHeat/accumAirflow;

    //Compute IRC power
    params->ircPower = rackIrc->getIRC()->computePower(accumAirflow);

    //Returning
    return params;
}

int MGHPCCPod::recomputeFS (std::string policy)
{
    int fanspeed=0; int numserver=0;
    
    std::map<std::string, RackAndIRC*>::iterator it;
    // Check temperature exceptions:
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        RackAndIRC *rackIrc = it->second;
        std::map<std::string, Rack>::iterator it2;
        for (it2=rackIrc->m_rack.begin(); it2!=rackIrc->m_rack.end(); ++it2){
            Rack &rack = it2->second;
            std::map<std::string, Server*>::iterator it3;
            for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
                Server *server = it3->second;
                //Now have iterator over all servers
                server->recomputeFS(policy);
                VLOG_1 << "Server: " <<  it3->first
                         << " fanSpeed: " << server->getFanSpeed();
                fanspeed+= server->getFanSpeed();
                numserver++;
            }
        }
    }
    LOG_INFO << "Average fan speed is: " << fanspeed/numserver;
    return (int)(fanspeed/numserver);
}

void MGHPCCPod::checkExceptions()
{
    std::map<std::string, RackAndIRC*>::iterator it;
    // Check temperature exceptions:
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        RackAndIRC *rackIrc = it->second;
        std::map<std::string, Rack>::iterator it2;
        for (it2=rackIrc->m_rack.begin(); it2!=rackIrc->m_rack.end(); ++it2){
            Rack &rack = it2->second;
            std::map<std::string, Server*>::iterator it3;
            for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
                Server *server = it3->second;
                bool high = server->checkMaxCPUTemp();
                if (high){
                    std::string msg = "Server " + it3->first + " reached "
                        + std::to_string(server->getCPUTemperature()[0])
                        + " degrees.";
                    std::string ex = std::string(MAX_TEMP_EX); 
                    addException(ex, msg);
                    return ;
                }
            }
        }
    }
}

void MGHPCCPod::getWorkloadAlloc(std::vector<Nodes> &alloc)
{
    alloc.clear();
    //Gathering data in alloc vector
    std::map<std::string, RackAndIRC*>::iterator it;
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        RackAndIRC *rackIrc = it->second;
        std::map<std::string, Rack>::iterator it2;
        for (it2=rackIrc->m_rack.begin(); it2!=rackIrc->m_rack.end(); ++it2){
            Rack &rack = it2->second;
            std::map<std::string, Server*>::iterator it3;
            for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
                Nodes node;
                //Parsing data...
                node.ircName = it->first;
                node.rackName = it2->first;
                node.serverName = it3->first;
                Server *server = it3->second;
                node.numCores = server->getUsedCPUs();
                node.cpuPower = server->getCpuDynPower();
                node.memPower = server->getMemPower();
                //Pushing to vector...
                alloc.push_back(node);
            }
        }
    }    
}

void MGHPCCPod::getIRCDynPower(std::map<std::string,double> &dynWkload)
{
    std::map<std::string, RackAndIRC*>::iterator it;
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        RackAndIRC *rackIrc = it->second;
        std::map<std::string, Rack>::iterator it2;
        double dynHeat = 0;
        for (it2=rackIrc->m_rack.begin(); it2!=rackIrc->m_rack.end(); ++it2){
            Rack &rack = it2->second;
            std::map<std::string, Server*>::iterator it3;
            for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
                Nodes node;
                //Parsing data...
                Server *server = it3->second;
                dynHeat+= server->getCpuDynPower();
                dynHeat+= server->getMemPower();
                dynHeat+= server->getIdlePower();
            }
        }
        //Pushing to map <irc name, heat>
        dynWkload.insert(std::make_pair(it->first, dynHeat));
    }
}

double MGHPCCPod::getAvgTinlet()
{
    std::map<std::string, RackAndIRC*>::iterator it;
    int acumInlet=0;
    for (it=m_coupled.begin(); it!=m_coupled.end(); ++it){
        RackAndIRC *rackIrc = it->second;
        int thisInlet = rackIrc->getIRC()->getInletTemp();
        acumInlet += thisInlet;
    }
    return ((double) (acumInlet/m_coupled.size()));
}
