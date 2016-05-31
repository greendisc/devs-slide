/*
 * ===================================================================
 *
 *       Filename:  irc.hh
 *    Description:  In-row cooler base class 
 *
 * ===================================================================
 */

#ifndef IRC_H
#define IRC_H

#include <vector>

#include "../generic_defs.h"

class IRC
{
public:

    static const int DEFAULT_INLET_TEMPERATURE = 18;
    static const int NUM_GALLONS_SETTING = 5;
    
    IRC();
    virtual ~IRC();

    // Setters and getters
    void setInletTemp(int temp);
    int getInletTemp();

    // Model computation
    virtual double computePower(double airflow) = 0;
    virtual double computeGallons(double airflow, double heatPower, double tempAirHot,
                                  double tempWaterCold) = 0;
    virtual double computeHotWaterTemp(double waterFlow, double heatPower) = 0;
    virtual int getGallons(int idx) = 0;
    
protected:
    int m_inletTemp;
    double m_tempWaterCold;
};



#endif /* IRC_H */
