/*
 * DevsPump.h
 *
 *  Created on: 29 mar. 2017
 *      Author: fernando
 */

#ifndef SRC_ROOM_DEVSPUMP_H_
#define SRC_ROOM_DEVSPUMP_H_
//STL
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <math.h>
//xDEVS
#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"


static constexpr double PUMP_DELTA_PRESSURE = 32.4;
static constexpr double PUMP_FACTOR = 0.435;
static constexpr double PUMP_EFFICIENCY = 0.65;

class DevsPump: public Atomic {
public:

	Port oPumpPower;
	std::map<std::string,Port*> iIRCGallons;


	DevsPump(const std::string& name, std::list<std::string>& puertos);
	virtual ~DevsPump();


	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

protected:
	double waterFlow;
	double IRCGallons;
	double pumpPower;
	unsigned int numIrcData;
	std::map<std::string, double> IRCGallonsMap;
	virtual void computePower();
	std::list<std::string> IRCNames;

};

#endif /* SRC_ROOM_DEVSPUMP_H_ */
