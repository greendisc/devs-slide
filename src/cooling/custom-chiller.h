/*
 * ===================================================================
 *
 *       Filename:  custom-chiller.hh
 *    Description:  Customized chiller plant
 *                  (free cooling, tower, chiller model) 
 *
 * ===================================================================
 */

#ifndef CUSTOM_CHILLER_H
#define CUSTOM_CHILLER_H

#include "chiller.h"

class CustomChiller : public Chiller
{
public:
    CustomChiller(const ChillerParams &params);
    virtual ~CustomChiller();

    // Model LUT's
    static constexpr double FREE_COOLING_WATER_TEMP = 18.3;
    static constexpr double CHILLER_WATER_TEMP = 12.7;

    // Chiller params
    static constexpr int MAX_COP_VALUE = 8;
    static constexpr double MIN_COP_VALUE = 2;
    
    // Heat exchanger
    static constexpr double HEAT_EXCHANGER_APPROACH = 1.1;
    
    //Tower params
    static constexpr double COOLING_TOWER_DESIGN_RANGE = 15;  //celsius
    static constexpr double COOLING_TOWER_DESIGN_APPROACH = 2.5;  //celsius    

    static constexpr double MAXIMUM_HEAT_TOWER = 85 ;         //ton
    static constexpr double COOLING_TOWER_WATERFLOW = 3*MAXIMUM_HEAT_TOWER ;    //gpm
    static constexpr double MAXIMUM_TOWER_FAN_POWER =
        0.2*MAXIMUM_HEAT_TOWER*HP_TO_WATT_CONVERSION ;  //Watt
    static constexpr double MAXIMUM_TOWER_AIRFLOW =
        1000*MAXIMUM_TOWER_FAN_POWER/HP_TO_WATT_CONVERSION ;    //cfm
    
    // Tower approach
    static const int NUM_RANGE_VALUES = 6;
    static const int NUM_WETBULB_SETTINGS = 7;
    
    static constexpr int rangeLUT[NUM_RANGE_VALUES]{
        100, 86, 71, 57, 43, 29
    };
    static constexpr double wetBulbLUT[NUM_WETBULB_SETTINGS]{
        -1.1, 4.4, 10.0, 15.6, 21.1, 26.7, 32.2
    };
    static constexpr double approachLUT[NUM_RANGE_VALUES][NUM_WETBULB_SETTINGS]{
        {11.9, 8.9, 6.7, 5.0, 3.5, 2.7, 2.2},
        {10.6, 7.8, 6.0, 4.3, 3.0, 2.4, 2.1},
        {9.2, 6.9, 5.3, 3.6, 2.5, 2.0, 1.8},
        {8.3, 6.1, 4.4, 2.9, 2.2, 1.8, 1.7},
        {6.7, 4.4, 3.3, 2.2, 1.9, 1.5, 1.5},
        {5.0, 3.3, 2.2, 1.6, 1.5, 1.4, 1.4}
    };
   
    // Power model
    virtual double computePower(CoolingParams *params);
    virtual bool reduceWaterTemp();

    // Cooling policy
    virtual double getMaxApproach(double tempOut);
    virtual double assertApproach(double tempOut);
    virtual double getFreeCoolingWaterTemp();
    virtual double getChillerWaterTemp();
    virtual double getTevapForMaxCOP(double tempOut, double load);
    virtual double getTowerRange();
    
private:
    double m_deltaTempAirTower;

    void computeChillerPower(double heat, CoolingParams *params);
    bool computeFreeCooling(double heat, CoolingParams *params);
    double getTowerApproach(double hotWaterTemp, double coldWaterTemp,
                            double wetBulbTemp);
    double getChillerCOP(double tempEvap, double tempCondenser);
    int computeWetBulbIdx(double wetBulbTemp);
    
};

#endif /* CUSTOM-CHILLER_H */
