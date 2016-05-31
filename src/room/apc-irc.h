/*
 * ===================================================================
 *
 *       Filename:  apc-irc.hh
 *    Description:  In-row cooler APC implementation 
 *
 * ===================================================================
 */

#ifndef APC_IRC_H
#define APC_IRC_H

#include "irc.h"

class ApcIrc : public IRC
{
public:
    ApcIrc();
    virtual ~ApcIrc();
    
    // Model LUT's
    static const int NUM_AIRFLOW_VALUES = 13;
        
    static constexpr int airflowLUT[NUM_AIRFLOW_VALUES] = {
        1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000
    };

    static constexpr int gallonsLUT[NUM_GALLONS_SETTING] = {
        5, 10, 15, 20, 25
    };

    static constexpr double perfLUT[NUM_AIRFLOW_VALUES][NUM_GALLONS_SETTING] = {
        {437.5, 560, 560, 560, 560},
        {562.5,	748.75,	780, 805, 811.25},
        {687.5,	937.5,	1000, 1050,	1062.5},
        {750, 1062.5, 1187.5, 1243.75, 1281.25},
        {812.5,	1187.5,	1375, 1437.5, 1500},
        {843.75, 1281.25, 1500, 1625, 1687.5},
        {875, 1375, 1625, 1812.5, 1875},
        {885.415, 1437.5, 1718.75, 1937.5, 2031.25},
        {895.83, 1500, 1812.5, 2062.5, 2187.5},
        {906.255, 1535, 1906.25, 2156.25, 2306.25},
        {916.68, 1570, 2000, 2250, 2425},
        {927.09, 1610, 2062.5, 2312.5, 2525},
        {937.5,	1650, 2125, 2375, 2625}
    };
    
    //Model computation (implements virtual functions)
    virtual double computePower(double airflow);
    virtual double computeGallons(double airflow, double heatPower, double tempAirHot,
                                  double tempWaterCold);
    virtual double computeHotWaterTemp(double waterFlow, double heatPower);
    virtual int getGallons(int idx);
private:
    int getAirflowIdx(double airflow);
    
};


#endif /* APC_IRC_H */

