/*
 * ===================================================================
 *
 *       Filename:  rack-irc.hh
 *    Description:  Rack+IRC coupling class 
 *
 * ===================================================================
 */
#ifndef RACK_IRC_H
#define RACK_IRC_H

#include <iostream>
#include "factory.h"
#include "irc.h"
#include "rack.h"

class RackAndIRC
{
public:

    typedef struct {
        std::string coupleName;
        int numRacksPerIRC;
        std::vector<Rack::RackParams> racks;
    }RackAndIRCParams;
    
    RackAndIRC(RackAndIRC::RackAndIRCParams &params, std::string IRCModel);
    virtual ~RackAndIRC();

    IRC* getIRC();
    std::map<std::string, Rack> &getRack();
    std::map<std::string, Rack> m_rack;

    void setIRCInletTemp( int inletTemp );
    
protected:
    IRC* m_irc;
};

#endif /* RACK_IRC_H */
