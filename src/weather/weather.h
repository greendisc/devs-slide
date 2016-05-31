/*
 * ===================================================================
 *
 *       Filename:  weather.hh
 *    Description:  Weather trace manager
 *
 * ===================================================================
 */

#ifndef _WEATHER_H_
#define _WEATHER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "../generic_defs.h"

class Weather
{
public:
    // Constructor and destructor
    Weather(std::ifstream &weatherFile);
    ~Weather();

    double getCurrentWeather(time_t now, bool &fi);
    
private:
    int m_lastTime;
    double m_lastTempValue;
    std::ifstream &m_file;
};


#endif /* _WEATHER_H_ */
