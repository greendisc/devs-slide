/*
 * DevsIrc.h
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSRACK_H_
#define SRC_XDEVS_DEVSRACK_H_

// STL
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <math.h>

// xDEVS
#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"

// OWN
#include "Job.h"

class DevsRack : public Atomic {
public:
	Port iJob;
	typedef struct{
		Port* iServersPowerIT;
		Port* iServersPowerFS;
		Port* iServersAirflow;
		Port* iServersTempOut;
		Port* iServersAvgTempCpu;
		Port*iServersMaxTempCpu;
		double iServersPowerITValue=-1;
		double iServersPowerFSValue=-1;
		double iServersAirflowValue=-1;
		double iServersTempOutValue=std::numeric_limits<double>::min();
		double iServersAvgTempCpuValue=std::numeric_limits<double>::min();
		double iServersMaxTempCpuValue=std::numeric_limits<double>::min();
	}tPorts;
	std::map<std::string, tPorts> ports;
	std::map<std::string, Port*> oJob;
	Port oRackPowerIT;
	Port oRackPowerFS;
	Port oRackAirflow;
	Port oRackTempOut;
	Port oRackAvgTempCpu;
	Port oRackMaxTempCpu;
	DevsRack(const std::string& name, std::list<std::string>& serverNames);
	virtual ~DevsRack();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

protected:
	std::list<std::string> serverNames;
	/*std::map<std::string, double> iServersPowerITMap;
	std::map<std::string, double> iServersPowerFSMap;
	std::map<std::string, double> iServersAirflowMap;
	std::map<std::string, double> iServersTempOutMap;
	std::map<std::string, double> iServersAvgTempCpuMap;
	std::map<std::string, double> iServersMaxTempCpuMap;*/
	Job* currentJob;
	double serversPowerIT;
	double serversPowerFS;
	double serversAirflow;
	double serversTempOut;
	double serversMaxTempCpu;
	double serversAvgTempCpu;
	unsigned int numPowerData;
};


#endif /* SRC_XDEVS_DEVSRACK_H_ */
