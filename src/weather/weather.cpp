/*
 * ===================================================================
 *
 *       Filename:  weather.cc
 *    Description:  Weather trace manager
 *
 * ===================================================================
 */

#include "weather.h"
#include <cstdlib>
#include <sstream>

Weather::Weather(std::ifstream &weatherFile)
    : m_lastTime(-1),
      m_lastTempValue(0.0),
      m_file(weatherFile)
{
    LOG_INFO << "Weather class created";
}

Weather::~Weather()
{
}

// @brief: this function looks for a temperature value given a time value.
//          
//         Weather file consists of:
//         "time(secs),temp(Celsius)"
double Weather::getCurrentWeather(time_t now, bool &fi)
{
    std::string line;
    while (m_lastTime < now){
        if (!getline(m_file, line)){
            LOG_WARNING << "Reached end of weather file ( lastTime=" << m_lastTime
                        << ", now= " << now << ")";
            fi=true;
            return m_lastTempValue;
        }
        std::stringstream ss(line);
        std::string aux;
        if (!std::getline(ss, aux, ',')){
            LOG_ERROR << "Error parsing time in weather file";
        }
        m_lastTime = atoi(aux.c_str());
        if (!std::getline(ss, aux, ',')){
            LOG_ERROR << "Error parsing temperature in weather file";
        }
        m_lastTempValue = (double) atof(aux.c_str());
    }
    return m_lastTempValue;
}

