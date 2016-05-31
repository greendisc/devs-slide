/*
 * ===================================================================
 *
 *       Filename:  rack-irc.cc
 *    Description:  Rack+IRC coupling class
 *
 * ===================================================================
 */

#include "rack-irc.h"

RackAndIRC::RackAndIRC( RackAndIRC::RackAndIRCParams &params, std::string IRCModel)
{
    //Server factory
    for (auto i=0; i<params.numRacksPerIRC; i++){
        Rack rack;
        std::map<std::string, std::string>::iterator it;
        for ( it=params.racks[i].servers.begin(); it!=params.racks[i].servers.end(); ++it){
            std::string serverName = it->first;
            std::string serverType = it->second;
            Server* pserv = ServerFactory::createServer(serverType);
            rack.m_servers.insert(std::make_pair(serverName, pserv));        
        }
        m_rack.insert(std::make_pair(params.racks[i].rackName, rack));
        
    }

    //IRC factory
    m_irc = IRCFactory::createIRC(IRCModel);
}

RackAndIRC::~RackAndIRC()
{
}

IRC* RackAndIRC::getIRC()
{
    return m_irc;
}

std::map<std::string, Rack> &RackAndIRC::getRack()
{
    return m_rack;
}

void RackAndIRC::setIRCInletTemp( int inletTemp )
{
    m_irc->setInletTemp(inletTemp);
    std::map<std::string, Rack>::iterator it;
    for (it=m_rack.begin(); it!=m_rack.end(); ++it){
        VLOG_2 << "Setting inlet temperature for servers in rack " << it->first;
        Rack &rack = it->second;
        std::map<std::string, Server*>::iterator it2;
        for (it2=rack.m_servers.begin(); it2!=rack.m_servers.end(); ++it2){
            VLOG_2 << "Setting inlet temperature of server " << it2->first << " to " << inletTemp;
            Server *server = it2->second;
            server->setInletTemp(inletTemp);
        }
    }
}

