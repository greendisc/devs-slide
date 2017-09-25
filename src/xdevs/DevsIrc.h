/*
 * DevsIrc.h
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSIRC_H_
#define SRC_XDEVS_DEVSIRC_H_

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

#define    CP_WATER     4.187      // (KJ/KgC)
#define    RHO_WATER    999.3      // (kg/m3)

#define    GPM_CONVERSION          0.00006389

// OWN
#include "Job.h"

class DevsIrc : public Atomic {
public:


    static const int NUM_AIRFLOW_VALUES = 13;
    static const int NUM_GALLONS_SETTING = 5;
    static const int airflowLUT[NUM_AIRFLOW_VALUES];
    static const int gallonsLUT[NUM_GALLONS_SETTING];
    static const double perfLUT[NUM_AIRFLOW_VALUES][NUM_GALLONS_SETTING];

	Port iJob;
	Port iWaterTemp;
	std::map<std::string, Port*> iRacksPowerIT;
	std::map<std::string, Port*> iRacksPowerFS;
	std::map<std::string, Port*> iRacksAirflow;
	std::map<std::string, Port*> iRacksTempOut;
	std::map<std::string, Port*> iRacksAvgTempCpu;
	std::map<std::string, Port*> iRacksMaxTempCpu;
	std::map<std::string, Port*> oJob;
	Port oIRCGallons;
	Port oIRCPower;
	Port oIRCITPower;
	Port oIRCFSPower;
	Port oIRCHotWater;
	Port oIRCAvgTempCpu;
	Port oIRCMaxTempCpu;

	DevsIrc(const std::string& name, std::list<std::string>& rackNames);
	virtual ~DevsIrc();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

	// OWN
	double computeGallons();
	double computePower();
	void computeHotWaterTemp();

protected:
	std::list<std::string> rackNames;
	std::map<std::string, double> iRacksPowerITMap;
	std::map<std::string, double> iRacksPowerFSMap;
	std::map<std::string, double> iRacksAirflowMap;
	std::map<std::string, double> iRacksTempOutMap;
	std::map<std::string, double> iRacksAvgTempCpuMap;
	std::map<std::string, double> iRacksMaxTempCpuMap;
	Job* currentJob;
	double coolerWaterTemp;
	double racksPowerIT;
	double racksPowerFS;
	double racksAirflow;
	double racksTempOut;
	unsigned int numPowerData;
	double racksAvgTout;
	double IRCGallons;
	double IRCPower;
	double deltaHotWater;
	double IRCMaxTempCpu;
	double IRCAvgTempCpu;
	double oldValues[4];
	int getAirflowIdx();
};


#endif /* SRC_XDEVS_DEVSIRC_H_ */
