/*
 * ===================================================================
 *
 *       Filename:  chiller.hh
 *    Description:  Chiller base class for power consumption simulator 
 *
 * ===================================================================
 */
#ifndef CHILLER_H
#define CHILLER_H

#include <iostream>

#include "../generic_defs.h"

class Chiller
{
public:

     // Chiller params    
    typedef struct {
        std::string chillerType;
        double coldWaterTemp ;
        double outdoorTemp;
    } ChillerParams;

    // Constructor
    Chiller(const ChillerParams &params);
    virtual ~Chiller();

    // Getters and setters
    double getPower();
    double getChillerPower();
    double getTowerPower();
    void setOutdoorTemp(double temp);
    void setWaterColdTemp(double temp);
    double getWaterColdTemp();
    
    // Power model
    virtual double computePower(CoolingParams *params) = 0;
    virtual bool reduceWaterTemp() = 0;
    
    // Cooling policy
    virtual double getMaxApproach(double tempOut) = 0;
    virtual double assertApproach ( double tempOut ) = 0;
    virtual double getFreeCoolingWaterTemp() = 0;
    virtual double getChillerWaterTemp() = 0;
    virtual double getTevapForMaxCOP(double tempOut, double load) = 0;
    virtual double getTowerRange() = 0;
    
protected:
    ChillerParams m_params;
    double m_chillerPower;
    double m_towerPower;
    double m_electricPower;    
};

#endif /* CHILLER_H */
