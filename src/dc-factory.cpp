/*
 * =====================================================================================
 *
 *       Filename:  dc-factory.cc
 *    Description:  DC Factory object files
 *
 * =====================================================================================
 */

#include "dc-factory.h"

Chiller* ChillerFactory::createChiller(const Chiller::ChillerParams &params)
{
    if (params.chillerType == "PlantBChiller"){
        return new PlantBChiller(params);
    }
    if (params.chillerType == "CustomChiller"){
        return new CustomChiller(params);
    }
    return NULL;
}

Pump* PumpFactory::createPump(const std::string &pumpType)
{
    if (pumpType == "ASHRAEPump"){
        return new ASHRAEPump();
    }
    return NULL;
}

Room* RoomFactory::createRoom(const std::string &roomType, const std::string &IRCModel,
                              std::vector<RackAndIRC::RackAndIRCParams> &rackIRC)
{
    if (roomType == "MGHPCCPod"){
        return new MGHPCCPod(IRCModel, rackIRC);
    }
    return NULL;
}

CoolingPolicy* PolicyFactory::createCoolingPolicy(const std::string &policyType, DCSimulator &sim)
{
    if (policyType == "budget"){
        LOG_INFO << "Using heat budget cooling policy";
        return new BudgetHeatPolicy(sim);
    }
    if (policyType == "fixed"){
        LOG_INFO << "Using fixed cooling policy";
        return new FixedInletPolicy(sim);
    }
    
    return NULL;
}

