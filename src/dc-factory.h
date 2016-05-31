/*
 * =====================================================================================
 *
 *       Filename:  dc-factory.hh
 *    Description:  DC Factory object files
 *
 * =====================================================================================
 */

#ifndef DC_FACTORY_H
#define DC_FACTORY_H

#include <iostream>
#include "cooling/ashrae-pump.h"
#include "cooling/chiller.h"
#include "cooling/custom-chiller.h"
#include "cooling/plantB-chiller.h"
#include "cooling/pump.h"
#include "policy/budget_heat.h"
#include "policy/cooling_policy.h"
#include "policy/fixed_inlet.h"
#include "room/mghpcc-pod.h"
#include "room/rack-irc.h"
#include "room/room.h"

class ChillerFactory
{
public:
    static Chiller* createChiller(const Chiller::ChillerParams &params);
};

class PumpFactory
{
public:
    static Pump* createPump(const std::string &pumpType);
};

class RoomFactory
{
public:
    static Room* createRoom(const std::string &roomType, const std::string &IRCModel,
                            std::vector<RackAndIRC::RackAndIRCParams> &rackIRC);
};

class PolicyFactory
{
public:
    static CoolingPolicy* createCoolingPolicy(const std::string &policyType, DCSimulator &sim);
};

#endif /* DC_FACTORY_H */
