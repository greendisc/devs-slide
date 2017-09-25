/*
 * DevsEnergyCalculator.h
 *
 *  Created on: 15 abr. 2017
 *      Author: fernando
 */

#ifndef SRC_ROOM_DEVSENERGYCALCULATOR_H_
#define SRC_ROOM_DEVSENERGYCALCULATOR_H_

#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"

#include <iostream>
#include <fstream>
#include <map>
#define NPORTS 3

class DevsEnergyCalculator: public Atomic {
public:
	typedef struct{
		double IRCPower;
		double pumpPower;
		double chillerPower;
		double towerPower;
		double weatherTemp;
		double IRCFSPower;
		double IRCITPower;
		double IRCAvgTempCpu;
		double IRCMaxTempCpu;
		bool print=false;

	}tPowerData;
	Port iPumpPower;
	Port iChillerPower;
	Port iTowerPower;
	std::map<std::string,Port*> iIRCPower;
	std::map<std::string,Port*> iIRCFSPower;
	std::map<std::string,Port*> iIRCITPower;
	std::map<std::string,Port*> iIRCAvgTempCpu;
	std::map<std::string,Port*> iIRCMaxTempCpu;
	Port iWeatherTemp;

	DevsEnergyCalculator(const std::string& name, std::list<std::string>& puertos, std::string salida);


	virtual ~DevsEnergyCalculator();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

protected:
	double sigma;
	double IRCPower;
	double IRCFSPower;
	double IRCITPower;
	double pumpPower;
	double chillerPower;
	double towerPower;
	double IRCAvgTempCpu;
	double IRCMaxTempCpu;
	double weatherTemp;
	unsigned int numData;
	std::map<std::string, double> IRCPowerMap;
	std::map<std::string, double> IRCFSPowerMap;
	std::map<std::string, double> IRCITPowerMap;
	std::map<std::string, double> IRCAvgTempCpuMap;
	std::map<std::string, double> IRCMaxTempCpuMap;
	std::map<double, tPowerData> PowerData;
	std::list<std::string> ircNames;
	std::string FichSalida;
};

#endif /* SRC_ROOM_DEVSENERGYCALCULATOR_H_ */
