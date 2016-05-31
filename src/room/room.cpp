/*
 * ===================================================================
 *
 *       Filename:  room.cc
 *    Description:  Room base class (creates and fills room)  
 *
 * ===================================================================
 */

#include "room.h"

constexpr char Room::MAX_TEMP_EX[];
constexpr char Room::PERF_EX[];

Room::Room()
{    
}

Room::~Room()
{    
}

Room::RoomPower& Room::getRoomPowerSummary()
{
    return m_powerSummary;
}

void Room::clearExceptions()
{
    m_powerSummary.roomException.clear();
    m_powerSummary.msg.clear();
}

void Room::addException(std::string &ex, std::string &msg)
{
    if (m_powerSummary.roomException.empty()){
        m_powerSummary.roomException = ex ;
        m_powerSummary.msg = msg;
        return;
    }
    m_powerSummary.roomException += "," + ex ;
    m_powerSummary.msg += "," + msg;
}

RackAndIRC* Room::getRackIrcCoupleByName(const std::string &name)
{
    return (m_coupled[name]);
}
