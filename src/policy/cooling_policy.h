/*
 * ===================================================================
 *
 *       Filename:  cooling_policy.hh
 *    Description:  Cooling policy main class
 *
 * ===================================================================
 */

#ifndef _COOLING_POLICY_H_
#define _COOLING_POLICY_H_

#include "../dc-simulator.h"

class CoolingPolicy
{
public:
    CoolingPolicy(DCSimulator &sim);
    virtual ~CoolingPolicy();

    virtual void run(double outTemp);

protected:
    DCSimulator &sim;
};

#endif /* _COOLING_POLICY_H_ */
