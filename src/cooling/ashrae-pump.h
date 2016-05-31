/*
 * ===================================================================
 *
 *       Filename:  ASHRAE-pump.hh
 *    Description:  Water pump implementation according to ASHRAE 
 *
 * ===================================================================
 */

#ifndef ASHRAE_PUMP_H
#define ASHRAE_PUMP_H

#include "../generic_defs.h"
#include "pump.h"

class ASHRAEPump : public Pump
{
public:
    ASHRAEPump();
    virtual ~ASHRAEPump();

    static constexpr double PUMP_DELTA_PRESSURE = 32.4;
    static constexpr double PUMP_FACTOR = 0.435;
    static constexpr double PUMP_EFFICIENCY = 0.65; 
    
    // Power model
    virtual double computePower(CoolingParams *params);
    
};



#endif /* ASHRAE_PUMP_H */

