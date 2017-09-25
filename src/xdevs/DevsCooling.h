/*
 * DevsCooling.h
 *
 *  Created on: 26/04/2017
 *      Author: roberto
 */

#ifndef SRC_XDEVS_DEVSCOOLING_H_
#define SRC_XDEVS_DEVSCOOLING_H_

// C++
#include <map>

#include <list>
//Devs
#include "../../lib/xdevs/xdevs/core/modeling/Coupled.h"

//Atomicos
#include "DevsPump.h"
#include "DevsChiller.h"

class DevsCooling: public Coupled {
public:
	DevsCooling(std::list<std::string>& ircNames);
	virtual ~DevsCooling();
	//PumpPorts
	Port oPumpPower;
	//sharedPort
	std::map<std::string,Port*> iIRCGallons;
	//Chiller ports
	std::map<std::string, Port*> iIRCHotWaterTemp;
	Port iWeatherTemp;

	Port oColdWaterTemp;
	Port oChillerPower;
	Port oTowerPower;
	Port oWeatherTemp;

};

#endif /* SRC_XDEVS_DEVSCOOLING_H_ */
