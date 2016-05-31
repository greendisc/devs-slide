/*
 * ===================================================================
 *
 *       Filename:  fixed_inlet.hh
 *    Description:  Fixed inlet cooling policy main class
 *
 * ===================================================================
 */

#ifndef _FIXED_INLET_H_
#define _FIXED_INLET_H_

#include "cooling_policy.h"

class FixedInletPolicy : public CoolingPolicy
{
public:
    FixedInletPolicy(DCSimulator &sim);
    virtual ~FixedInletPolicy();

    virtual void run(double tempOut);
};

#endif /* _FIXED_INLET_H_ */
