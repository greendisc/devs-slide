/*
 * ===================================================================
 *
 *       Filename:  budget_heat_policy.hh
 *    Description:  Cooling policy main class
 *
 * ===================================================================
 */

#ifndef _BUDGET_HEAT_H_
#define _BUDGET_HEAT_H_

#include "cooling_policy.h"

class BudgetHeatPolicy : public CoolingPolicy
{
public:

    typedef struct{
        int tinlet;
        int fanSpeed;
        double hotWaterTemp;
        Room::RackParams power;
    } ircCoolConfig;
    
    BudgetHeatPolicy(DCSimulator &sim);
    virtual ~BudgetHeatPolicy();

    static constexpr int TEMP_ACCURACY = 1; //degree
    static constexpr int KW_ACCURACY = 100; //watts
    
    //Constants for heat budgetting - TODO?
    static const int NUM_INLET_SETTING = 7;
    static constexpr int inletSettings[NUM_INLET_SETTING] = { 18, 20, 22, 24, 26, 28, 30};

    virtual void run(double tempOut);
    
private:
    bool assertApproach(double tempOut);
    double computeHotWaterCap( double load );
    void budgetHotWater(double totHeat, std::map<std::string, double> &heat,
                        double maxHotWater,
                        std::map<std::string, double> &ircMaxHotWater,
                        bool freeCooling);
    void getHotWaterCapCandidates(const std::string &ircName, double thisMaxHotWater,
                                  std::vector<ircCoolConfig> &setups);

    void getMinTevapCandidates( const std::string &ircName, double thisMinHotWater,
                                std::vector<ircCoolConfig> &okSetups,
                                std::vector<ircCoolConfig> &koSetups);
    bool hitsMaxTemperature(RackAndIRC* rackPtr, ircCoolConfig conf);
    
    void setCoolingConfig( std::map<std::string, ircCoolConfig> &conf);
    
};

#endif /* _BUDGET_HEAT_H_ */
