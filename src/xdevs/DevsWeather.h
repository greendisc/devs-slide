/*
 * DevsWeather.h
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSWEATHER_H_
#define SRC_XDEVS_DEVSWEATHER_H_

#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"

#include <iostream>
#include <fstream>

class DevsWeather : public Atomic {
public:
	typedef struct {
		long second;
		double tempIncrement;
	} WeatherEntry;

	Port oStop;
	Port oOut;
	DevsWeather(const std::string& name, const std::string& weatherFilePath);
	virtual ~DevsWeather();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();
protected:
	bool fi;
	std::ifstream weatherFile;
	WeatherEntry* nextWeatherEntry;
	WeatherEntry* getNextEntry();
	double nextDiff;
};

#endif /* SRC_XDEVS_DEVSWEATHER_H_ */
