/*
 * ===================================================================
 *
 *       Filename:  plantB-chiller.hh
 *    Description:  PlantB Chiller model 
 *
 * ===================================================================
 */

#ifndef PLANTB_CHILLER_H
#define PLANTB_CHILLER_H

#include "chiller.h"

class PlantBChiller : public Chiller
{
public:
    PlantBChiller(const ChillerParams &params);
    virtual ~PlantBChiller();

    //Model LUTs    
    static const int NUM_WATERCOLD_TEMPS = 7;

    static constexpr double waterColdLUT[NUM_WATERCOLD_TEMPS] {
        7.2, 10, 12.7, 15.5, 18.3, 21.1, 23.9
    };

    static constexpr double perfLUT[NUM_WATERCOLD_TEMPS] {
        0.5, 0.45, 0.37, 0.3, 0.25, 0.15, 0.1
    };
    
    // Power model
    virtual double computePower(CoolingParams *params);
    virtual bool reduceWaterTemp();

    // Cooling policy
    virtual double getMaxApproach(double tempOut) ;
    virtual double assertApproach ( double tempOut ) ;
    virtual double getFreeCoolingWaterTemp() ;
    virtual double getChillerWaterTemp() ;
    virtual double getTevapForMaxCOP(double tempOut, double load);
    virtual double getTowerRange();
    
private:
    double getEfficiency(double tempColdWater);
};


#endif /* PLANTB_CHILLER_H */

