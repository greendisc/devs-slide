/*
 * ===================================================================
 *
 *       Filename:  pump.hh
 *    Description:  Pump base class for power consumption simulator
 *
 * ===================================================================
 */

#ifndef PUMP_H
#define PUMP_H

#include "../generic_defs.h"

class Pump
{
public:
    Pump();
    virtual ~Pump();

    //Getters and setters
    double getPower();

    // Power model
    virtual double computePower(CoolingParams *params) = 0;
    
protected :
    double m_electricPower;
    
};


#endif /* PUMP_H */
