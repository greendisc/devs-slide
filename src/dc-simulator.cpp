/*
 * =====================================================================================
 *
 *       Filename:  dc-simulator.cc
 *    Description:  Overall-DC power consumption simulator 
 *
 * =====================================================================================
 */

#include "dc-simulator.h"

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "dc-factory.h"

#include "generic_defs.h"
#include "parser/dc-parser.h"

extern bool noLog;

/**
   constructing simulator class
*/
DCSimulator::DCSimulator(DCParams &layout)
{
    // Creating DC objects
    m_chiller = ChillerFactory::createChiller(layout.chiller);
    if(m_chiller == NULL){
        LOG_FATAL << "Chiller not created";
    }
    m_pump = PumpFactory::createPump(layout.pumpType);
    if(m_pump == NULL){
        LOG_FATAL << "Pump not created";
    }
    m_room = RoomFactory::createRoom(layout.roomType, layout.ircType, layout.rackIRC);
    if(m_room == NULL){
        LOG_FATAL << "Room not created";
    }
}

DCSimulator::~DCSimulator()
{
}

Room* DCSimulator::getRoom()
{
    return m_room;
}
Chiller* DCSimulator::getChiller()
{
    return m_chiller;
}

void DCSimulator::run()
{       
}

void DCSimulator::computePower( double outdoorTemp )
{
    m_chiller->setOutdoorTemp(outdoorTemp);
    computePower();
}

void DCSimulator::computePower()
{
    // We compute total power until we reach convergence
    VLOG_1 << "We compute total power until we reach convergence";
    bool converged=false;
    int it =0;
    while (!converged){
        it++;
        // Compute power consumption of the room, and extract
        // data to feed to cooling subsystems 
        VLOG_1 << "Going to compute IT power... (iteration=" << it << ")";
        CoolingParams *cooling = m_room->computePower(m_chiller->getWaterColdTemp());
        
        VLOG_1 << "Checking for room exceptions...";
        m_room->checkExceptions();

        if (!cooling){
            LOG_ERROR << "Cooling parameters for room could not be calculated";
            bool res = m_chiller->reduceWaterTemp();
            if (!res){
                VLOG_1 << "Solution did not converge";
                break;
            }
            continue;
        }
        VLOG_1 << "Going to compute Pump power...";
        m_pump->computePower(cooling);
        VLOG_1 << "Going to compute Chiller power...";
        m_chiller->computePower(cooling);
        converged = cooling->converged;
        
        if (!converged){
            bool res = m_chiller->reduceWaterTemp();
            if (!res){
                VLOG_1 << "Solution did not converge";
                break;
            }
        }
    }
}

int DCSimulator::applyFSPolicy(std::string policy)
{
    VLOG_2 << "Will recompute FS for all servers in the room";
    int fs = m_room->recomputeFS(policy);
    return fs;
}

void DCSimulator::printAllPowerValues( time_t time, double tempOut, std::ofstream &powerOutFile )
{
    VLOG_1 << "Power values (CSV) in kW";
    std::cout << "'Time','Tout','IT','FS','IRC','Pump','Chiller','Tower','Total',"
              << "['AvgTinlet']['Exception'],['Message']"
              << std::endl ;
    
    Room::RoomPower roomSummary = m_room->getRoomPowerSummary();
    double totPower = roomSummary.serverITPower + roomSummary.serverFSPower 
        + roomSummary.ircPower + m_pump->getPower()
        + m_chiller->getChillerPower() + m_chiller->getTowerPower();
    double avgTinlet = m_room->getAvgTinlet();
    
    std::cout << time << "," << tempOut << ","
              << roomSummary.serverITPower/1000 << ","
              << roomSummary.serverFSPower/1000 << ","
              << roomSummary.ircPower/1000 << ","
              << m_pump->getPower()/1000 << ","
              << m_chiller->getChillerPower()/1000 << ","
              << m_chiller->getTowerPower()/1000 << ","
              << totPower/1000 << ","
              << avgTinlet ;
        
    powerOutFile << time << "," << tempOut << ","
                 << roomSummary.serverITPower/1000 << ","
                 << roomSummary.serverFSPower/1000 << ","
                 << roomSummary.ircPower/1000 << ","
                 << m_pump->getPower()/1000 << ","
                 << m_chiller->getChillerPower()/1000 << ","
                 << m_chiller->getTowerPower()/1000 << ","
                 << totPower/1000 << ","
                 << avgTinlet ;

    if (!roomSummary.roomException.empty()){
        std::cout << ",'" << roomSummary.roomException << "','"
                  << roomSummary.msg << "'" ;
        powerOutFile << ",'" << roomSummary.roomException << "','"
                     << roomSummary.msg << "'" ;
    }
    if (noLog){
        std::cout << ",'NoLog'" ;
        powerOutFile << ",'NoLog'" ;
    }

    std::cout << std::endl ;
    powerOutFile << std::endl;
}

void DCSimulator::printEnergyStatistics(std::string &file)
{
    std::ifstream powerFile(file);
    time_t lastTstamp = 0; time_t thisTstamp = 0; 
    if (!powerFile){
        LOG_ERROR << "Could not open power file, cannot print energy statistics";
        return ;
    }
    VLOG_2 << "Will print energy statistics";
    std::string line;
    int month=1;
    time_t accumTime=0;
    double tempOut;
    double thispower=0;
    double accumEnergy[NUMSTATISTICS];
    memset(accumEnergy, 0, sizeof(accumEnergy));
    
    while (std::getline (powerFile, line)){
        // read power file line by line and compute energy
        std::stringstream ss(line);
        std::string aux;
        if (!std::getline(ss, aux, ',')){
            LOG_ERROR << "Error parsing tstamp";
        }
        thisTstamp = (time_t) atoi(aux.c_str());
        if (!std::getline(ss, aux, ',')){
            LOG_ERROR << "Error parsing outdoor temperature";
        }
        tempOut = (double) atof(aux.c_str());
        if ((accumTime / EVERYSECS) > month ){
            std::cout << month << "," << tempOut;
            
        }
        int i=0;
        while (std::getline(ss, aux, ',')){
            if (i> NUMSTATISTICS){
                LOG_WARNING << "Enough statistics processed";
                break;
            }
            thispower = atof (aux.c_str());
            accumEnergy[i]+= thispower*(thisTstamp-lastTstamp);
            if ((accumTime / EVERYSECS) > month ){
                //printing energy statistics
                std::cout << accumEnergy[i];
            }
            i++;
        }
        accumTime+=thisTstamp;
        lastTstamp=thisTstamp;
        // check if it's time to print a new line and reset counters
        if ((accumTime / EVERYSECS) > month ){
            //printing energy statistics
            std::cout << std::endl ;
            VLOG_2 << "Line finished!!";
            accumTime=0;
            memset(accumEnergy, 0, sizeof(accumEnergy));
            month++;   
        }
    }
    VLOG_2 << "Printing last energy values.";
    std::cout << "end," << tempOut;
    for (unsigned int j=0; j< NUMSTATISTICS; j++){
        std::cout << "," << accumEnergy[j];
    }
    std::cout << std::endl;
    VLOG_2 << "Flushing stdout...";
    std::cout.flush();
}

