/*
 * ===================================================================
 *
 *       Filename:  factory.cc
 *    Description:   
 *
 * ===================================================================
 */

#include "factory.h"

Server* ServerFactory::createServer( const std::string &serverType )
{
    if (serverType == "solana"){
        return new SolanaServer();
    }
    if (serverType == "decathlete"){
        return new DecathleteServer();
    }
    return NULL;
}

IRC* IRCFactory::createIRC( const std::string &IRCModel )
{
    if (IRCModel == "APC"){
        return new ApcIrc();
    }
    return NULL;
}
