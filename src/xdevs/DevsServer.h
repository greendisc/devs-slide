/*
 * DevsIrc.h
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSSERVER_H_
#define SRC_XDEVS_DEVSSERVER_H_

// STL
#include <list>
#include <map>
#include <cmath>
#include <iostream>

// xDEVS
#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"
// OWN
#include "Job.h"

#define NUM_FAN_SPEED 4

class DevsServer : public Atomic {
public:
	static const int fanSpeedLUT[NUM_FAN_SPEED];
	static const double fanPowerLUT[NUM_FAN_SPEED];
	static const double tempCpu0LUT[NUM_FAN_SPEED][2];
	static const double tempCpu1LUT[NUM_FAN_SPEED][2];
	Port iJob;
	Port oServerPowerIT;
	Port oServerPowerFS;
	Port oServerAirflow;
	Port oServerTempOut;
	Port oServerAvgTempCpu;
	Port oServerMaxTempCpu;

	DevsServer(const std::string& name);
	virtual ~DevsServer();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

	// EXTRA:
	void updateState();
	void setInlet(int tInlet);
	void setInitialValues(const double values[4]);
	int getIndex(int fanSpeed);

protected:
	Job* currentJob;
	double cpuPower;
	double memPower;
	int cpusUsed; // Number of CPUs used
	int tempInlet; // TODO-JOSELE: Comprobar si este atributo sobra
	double cpusTemp[2]; // CPU (0,1) temperature, TODO-JOSELE: Comprobar si estos atributos sobran
	double serverPowerIT;
	double serverPowerFS;
	double serverAirflow;
	double serverTempOut;
	double fanSpeed;
	int fanSpeedidx;
};


#endif /* SRC_XDEVS_DEVSSERVER_H_ */
