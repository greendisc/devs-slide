/*
 * ===================================================================
 *
 *       Filename:  factory.hh
 *    Description:  Server, IRC and other elements factory 
 *
 * ===================================================================
 */

#ifndef FACTORY_H
#define FACTORY_H

#include "apc-irc.h"
#include "decathlete-server.h"
#include "irc.h"
#include "rack-irc.h"
#include "server.h"
#include "solana-server.h"

class ServerFactory
{
public:
    static Server* createServer( const std::string &serverType );
};

class IRCFactory
{
public:
    static IRC* createIRC( const std::string &IRCModel );
};

#endif /* FACTORY_H */

