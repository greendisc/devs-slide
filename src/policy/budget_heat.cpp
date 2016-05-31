/*
 * ===================================================================
 *
 *       Filename:  budget_heat_policy.cc
 *    Description:  Cooling policy main class
 *
 * ===================================================================
 */

#include "budget_heat.h"

constexpr int BudgetHeatPolicy::inletSettings[];

BudgetHeatPolicy::BudgetHeatPolicy(DCSimulator &sim)
    : CoolingPolicy(sim)
{
    LOG_INFO << "BudgetHeat cooling control policy working";
}

BudgetHeatPolicy::~BudgetHeatPolicy()
{
}

void BudgetHeatPolicy::run( double tempOut )
{
    bool freeCooling = false;

    // Step 1:
    //--------
    // Checking if there is any chance of using free cooling
    // (i.e. outdoor temperature lower than cold water temperature)
    double approach = sim.getChiller()->assertApproach(tempOut);

    // Get dynamic power per IRC (to budget water proportionally to load)
    std::map<std::string, double> dynHeat;
    sim.getRoom()->getIRCDynPower(dynHeat);
    double totDynHeat = 0;
    std::map<std::string, double>::iterator it;
    for( it = dynHeat.begin(); it != dynHeat.end(); it++) {
        totDynHeat+=it->second;
    }

    std::map<std::string, ircCoolConfig> bestCoolingConfig;
    if (approach > 0){
        VLOG_1 << "Approach assertion is true";
        
        // Step 2: heat budgeting
        //-----------------------
        // Get maximum Tcwhot and dT to be able to use free cooling
        double deltaHotWaterMax = computeHotWaterCap(approach);
        std::map<std::string, double> ircMaxHotWater;
        budgetHotWater(totDynHeat, dynHeat, deltaHotWaterMax, ircMaxHotWater, true);

        // Step 3: obtain set of candidate configs
        //----------------------------------------
        // Now we have a vector ircMaxHotWater that tells us the maximum
        // hot water temperature for each rack.
        // -- The goal now is to find the set of configurations that meet the cap
        VLOG_1 << "DynHeat vector has size= " << dynHeat.size();
        if (!dynHeat.size()){
            LOG_FATAL << "DynHeat vector has size 0. Something did not work!";
        }
        for (it=dynHeat.begin(); it !=dynHeat.end(); it++){
            double thisMaxHotWater = ircMaxHotWater[it->first];
            std::vector<ircCoolConfig> setups;

            //Getting candidates
            getHotWaterCapCandidates(it->first, thisMaxHotWater, setups);
            VLOG_1 << "Found " << setups.size() << " for IRC " << it->first;
            if (!setups.size()){
                // If the vector is empty, then we cannot meet the cap 
                // and we need to turn the chiller on --> Step 5
                VLOG_1 << "IRC " << it->first << " does not meet the cap.";
                break; 
            }
            
            // Step 4: we get the configuration that yields the minimum
            // power consumption (IT+Fan+IRC)
            //----------------------------------------------------------
            // remember who setups is: std::vector<ircCoolConfig> setups;
            ircCoolConfig best = setups[0];
            double bestPower = best.power.ircPower + best.power.serverFSPower
                + best.power.serverITPower;
            for (unsigned int i=1; i< setups.size(); i++){
                double thisPower = setups[i].power.ircPower
                    + setups[i].power.serverFSPower + setups[i].power.serverITPower;
                if (thisPower < bestPower){
                    bestPower = thisPower;
                    best = setups[i];
                }
            }
            // We store the best configuration
            bestCoolingConfig.insert(std::make_pair(it->first, best));            
        }
        // Applying cooling policy in the simulator and returning!!
        if (bestCoolingConfig.size() == dynHeat.size()){
            VLOG_1 << "Will apply free cooling policy";
            setCoolingConfig(bestCoolingConfig);
            return;
        }
        VLOG_1 << "Did not find a config setup in free cooling for all IRC. "
               << "Will try with chiller";
    }
    VLOG_1 << "Approach assetion is false (no free cooling)";

    // Step 5: minimize chiller burden --> find set of configs
    //--------------------------------------------------------
    // If we are here, it's because we cannot use free cooling
    // We try to minimize chiller burden
    // We find the minium tevapOpt that maximizes chiller COP
    double minTevap = sim.getChiller()->getTevapForMaxCOP(tempOut, approach);
    double deltaMinTevap = minTevap - sim.getChiller()->getChillerWaterTemp();
    if (deltaMinTevap < 0){
        LOG_WARNING << "Minimum delta evap temperature is negative! this is an error";
        return;
    }
    VLOG_1 << "minTevapForMaxCOP= " << minTevap << " delta= " << deltaMinTevap;
    std::map<std::string, double> ircMinHotWater;
    budgetHotWater(totDynHeat, dynHeat, minTevap, ircMinHotWater, false);

    // Step 6: Find set of configs
    //----------------------------
    // Now we find the configurations that exceed tevapMin 
    // --> reduce chiller burden
    // --> note that we need at least one config! 
    for (it=dynHeat.begin(); it !=dynHeat.end(); it++){
        double thisMinHotWater = ircMinHotWater[it->first] + sim.getChiller()->getChillerWaterTemp();
        std::vector<ircCoolConfig> okSetups, koSetups;
        //Getting candidates
        getMinTevapCandidates(it->first, thisMinHotWater, okSetups, koSetups);
        VLOG_1 << "Size of okSetups: " << okSetups.size()
               << " and of koSetups: " << koSetups.size();
        
        // Step 7: Find best config
        //-------------------------
        // From the previous configurations, we select the
        // one that exceeds tevapMin by the minimum (reduce other power consumption)
        if (!okSetups.size()){
            VLOG_1 << "No available candidates";
            // If the vector of setups that meet the cap is empty, then we need to
            // pickup the setup that maximizes COP (i.e. highest tevap)
            if (!koSetups.size()){
                LOG_ERROR << "No available KO setups! This is an error!";
                return; 
            }
            ircCoolConfig best = koSetups[0];
            for (unsigned int i=1; i< koSetups.size(); i++){
                double bestDelta = best.hotWaterTemp;
                double thisDelta = koSetups[i].hotWaterTemp;
                VLOG_2 << "Tinlet= " << koSetups[i].tinlet << " FS= "
                       << koSetups[i].fanSpeed << " thisDelta= " << thisDelta
                       << " (bestDelta= " << bestDelta << ")";
                //Check temperature within a range
                if ( thisDelta > bestDelta ){
                    //Check power, and select minimum
                    if (( thisDelta - bestDelta ) <= TEMP_ACCURACY){
                        double thisPower = koSetups[i].power.serverITPower
                            + koSetups[i].power.serverFSPower
                            + koSetups[i].power.ircPower;
                    
                        double bestPower = best.power.serverITPower
                            + best.power.serverFSPower + best.power.ircPower; 
                        if ( thisPower > (bestPower + KW_ACCURACY )){
                            continue;
                        }
                    }
                    best = koSetups[i];
                    bestDelta = thisDelta;
                }
            }
            bestCoolingConfig.insert(std::make_pair(it->first, best));
        }
        else {
            //Finding config that exceeds tevapMin by the minimum
            ircCoolConfig best = okSetups[0];
            for (unsigned int i=1; i<okSetups.size();i++){
                double bestDelta = best.hotWaterTemp - thisMinHotWater;
                double thisDelta = okSetups[i].hotWaterTemp - thisMinHotWater;
                if (thisDelta < bestDelta){
                    if ( (bestDelta - thisDelta) <= TEMP_ACCURACY ){
                        //Check power, and select minimum
                        double thisPower = okSetups[i].power.serverITPower
                            + okSetups[i].power.serverFSPower
                            + okSetups[i].power.ircPower;
                        
                        double bestPower = best.power.serverITPower
                            + best.power.serverFSPower + best.power.ircPower; 
                        if ( thisPower > (bestPower + KW_ACCURACY ) ){
                            continue;
                        }
                    }
                    best = okSetups[i];
                    bestDelta = thisDelta;
                }
            }
            bestCoolingConfig.insert(std::make_pair(it->first, best));
        }
    }
    setCoolingConfig(bestCoolingConfig);
    return;
}

double BudgetHeatPolicy::computeHotWaterCap( double load )
{
    // The maximum load corresponds to a certain amount of degrees
    double degs = load * sim.getChiller()->getTowerRange() / 100;
    double maxHotWater = sim.getChiller()->getFreeCoolingWaterTemp() + degs;
    
    VLOG_1 << "Maximum Hot water temperature at tower is: " << maxHotWater
           << " (degs=" << degs << " , load= " << load << ")";
    return maxHotWater;
}

void BudgetHeatPolicy::budgetHotWater(double totHeat, std::map<std::string, double> &heat,
                                      double maxHotWater, std::map<std::string, double> &ircMaxHotWater,
                                      bool freeCooling)
{
    // Fair-schedule fashion
    //----------------------
    // Compute delta water temperature
    double deltaWater;
    if (freeCooling){
        deltaWater= maxHotWater - sim.getChiller()->getFreeCoolingWaterTemp();
    }
    else {
        deltaWater= maxHotWater - sim.getChiller()->getChillerWaterTemp();
    }
    std::map<std::string, double>::iterator it;
    for (it = heat.begin(); it!= heat.end(); it++){
        double portion = ((it->second)*heat.size())/totHeat;
        double thisDelta = portion*deltaWater;
        ircMaxHotWater.insert(std::make_pair(it->first, thisDelta));
        VLOG_1 << "Hot water delta for " << it->first << " is: " << thisDelta;
    }
}

//@brief: setups will contain the candidate setups
void BudgetHeatPolicy::getHotWaterCapCandidates(const std::string &ircName,
                                                double thisMaxHotWater,
                                                std::vector<ircCoolConfig> &setups)
{
    RackAndIRC* rackPtr = sim.getRoom()->getRackIrcCoupleByName(ircName);
    if (!rackPtr){
        LOG_WARNING << "Could not find IRC " << ircName << " by name!";
        return ;
    }
    VLOG_1 << "Num inlet settings: " << NUM_INLET_SETTING
           << " and fan speed " << Server::NUM_FAN_SPEED ;
    for (unsigned int i=0; i<NUM_INLET_SETTING; i++){
        for (unsigned int j=0; j< Server::NUM_FAN_SPEED; j++){
            // set inlet temperature to IRC
            int inletTemp = inletSettings[i];
            rackPtr->setIRCInletTemp(inletTemp);
            // Compute IT + Fan + IRC power in the Rack-IRC couple
            Room::RackParams *rackParams = sim.getRoom()->
                computeRackPower(rackPtr, j);
            // Find hot water temperature for specific gallons
            double gallons = rackPtr->getIRC()->
                computeGallons(rackParams->airflow, rackParams->serverITPower,
                               rackParams->avgTout,
                               sim.getChiller()->getFreeCoolingWaterTemp());
            if (gallons < 0){
                VLOG_2 << "This performance cannot be achieved";
                continue;
            }
            double hotWater = rackPtr->getIRC()->
                computeHotWaterTemp(gallons, rackParams->serverITPower);

            VLOG_1 << "Setup for " << ircName
                   << ": inlet=" << inletSettings[i]
                   << " fanSpeed= " << j << " gallons= " << gallons
                   << " hotWater= " << hotWater
                   << " thisMaxHotWater= " << thisMaxHotWater ;
            // Checking hot water condition (needed for free cooling)
            if ( (hotWater > 0) &&
                 (hotWater < (thisMaxHotWater
                              + sim.getChiller()->getFreeCoolingWaterTemp())) ){
                VLOG_1 << "... is candidate setup" ;
                // Fill-in cfg struct
                ircCoolConfig cfg ={};
                cfg.tinlet = inletSettings[i];
                cfg.fanSpeed = j;
                cfg.hotWaterTemp = hotWater;
                cfg.power = *(rackParams);
                VLOG_1 << "Hot water temp: " << hotWater
                       << " ITpower=" << cfg.power.serverITPower
                       << " FanPower=" << cfg.power.serverFSPower
                       << " IRCPower=" << cfg.power.ircPower ;

                // Before adding the setup, check for temperature redlining!!
                if (hitsMaxTemperature(rackPtr, cfg)){
                    VLOG_1 << "Setup hits max temperature";
                    continue;
                }
                // If everything is correct, store setup
                setups.push_back(cfg);
            }         
        }
    }
}

void BudgetHeatPolicy::getMinTevapCandidates( const std::string &ircName,
                                              double thisMinHotWater,
                                              std::vector<ircCoolConfig> &okSetups,
                                              std::vector<ircCoolConfig> &koSetups)
{
    RackAndIRC* rackPtr = sim.getRoom()->getRackIrcCoupleByName(ircName);
    if (!rackPtr){
        LOG_WARNING << "Could not find IRC " << ircName << " by name!";
        return ;
    }
    for (unsigned int i=0; i<NUM_INLET_SETTING; i++){
        for (unsigned int j=0; j< Server::NUM_FAN_SPEED; j++){
            // set inlet temperature to IRC
            int inletTemp = inletSettings[i];
            rackPtr->setIRCInletTemp(inletTemp);
            // Compute IT + Fan + IRC power in the Rack-IRC couple
            Room::RackParams *rackParams = sim.getRoom()->
                computeRackPower(rackPtr, j);
            // Find hot water temperature for specific gallons
            double gallons = rackPtr->getIRC()->
                computeGallons(rackParams->airflow, rackParams->serverITPower,
                               rackParams->avgTout,
                               sim.getChiller()->getChillerWaterTemp());
            if (gallons < 0){
                VLOG_2 << "This performance cannot be achieved";
                continue;
            }
            double hotWater = rackPtr->getIRC()->
                computeHotWaterTemp(gallons, rackParams->serverITPower);
            
            // Checking the COP achieved when using this hot water
            bool candidate = false;
            if ( (hotWater > 0) && (hotWater >= thisMinHotWater) ){
                VLOG_1 << "Candidate setup for " << ircName
                       << ": inlet=" << inletSettings[i]
                       << " fanSpeed=" << j ;
                candidate=true;
            }
            // Fill-in cfg struct
            ircCoolConfig cfg ={};
            cfg.tinlet = inletSettings[i];
            cfg.fanSpeed = j;
            cfg.hotWaterTemp = hotWater;
            cfg.power = *(rackParams);
            VLOG_1 << "Hot water temp: " << hotWater
                   << " (minHotWater= " << thisMinHotWater
                   << ") ITpower=" << cfg.power.serverITPower
                   << " FanPower=" << cfg.power.serverFSPower
                   << " IRCPower=" << cfg.power.ircPower ;
            
            // Before adding the setup, check for temperature redlining!!
            // If hits max temp, then setup is not good
            if (hitsMaxTemperature(rackPtr, cfg)){
                VLOG_1 << "Setup hits max temperature";
                continue;
            }
            // If everything is correct, store setup in either "ok" or "ko"
            if (candidate){
                okSetups.push_back(cfg);
            }
            else {
                koSetups.push_back(cfg);
            }
        }
    }
}

bool BudgetHeatPolicy::hitsMaxTemperature(RackAndIRC* rackPtr, ircCoolConfig conf)
{
    std::map<std::string, Rack>::iterator it2;
    for (it2=rackPtr->getRack().begin(); it2!=rackPtr->getRack().end(); ++it2){
        Rack &rack = it2->second;
        std::map<std::string, Server*>::iterator it3;
        for (it3=rack.m_servers.begin(); it3!=rack.m_servers.end(); ++it3){
            Server *server = it3->second;
            bool high = server->checkMaxCPUTemp();
            if (high){
                return true;
            }
        }
    }
    return false;
}

void BudgetHeatPolicy::setCoolingConfig( std::map<std::string, ircCoolConfig> &conf)
{
    // Need to iterate through IRC setting Inlet, FS, and recomputing power
    // (even though power will be recomputed again when needed)
    std::map<std::string, ircCoolConfig>::iterator it;
    VLOG_1 << "Config size is: " << conf.size();
    for (it=conf.begin(); it!=conf.end(); it++){
        RackAndIRC* rackPtr = sim.getRoom()->getRackIrcCoupleByName(it->first);
        if (!rackPtr){
            LOG_WARNING << "Could not find IRC " << it->first << " by name!";
            return ;
        }
        rackPtr->setIRCInletTemp(it->second.tinlet);
        // Compute IT + Fan + IRC power in the Rack-IRC couple
        sim.getRoom()->computeRackPower(rackPtr, it->second.fanSpeed);
        VLOG_1 << "CoolingConfig: " << it->first << " : Tinlet= " << it->second.tinlet
               << " FS= " << it->second.fanSpeed << " HotWater= " << it->second.hotWaterTemp ; 
    }
}

