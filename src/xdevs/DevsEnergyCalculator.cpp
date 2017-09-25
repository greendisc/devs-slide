/*
 * DevsEnergyCalculator.cpp
 *
 *  Created on: 15 abr. 2017
 *      Author: fernando
 */

#include "DevsEnergyCalculator.h"

DevsEnergyCalculator::DevsEnergyCalculator(const std::string& name,
		std::list<std::string>& puertos, std::string salida)
		: Atomic(name), iPumpPower("PumpPower"), iChillerPower("ChillerPower"), iTowerPower(
				"TowerPower"), iIRCPower(), iWeatherTemp("WeatherTemp"), PowerData() {

	this->addInPort(&iPumpPower);
	this->addInPort(&iChillerPower);
	this->addInPort(&iTowerPower);
	for (std::list<std::string>::iterator it = puertos.begin();
			it != puertos.end(); ++it) {
		Port* port = new Port(*it);
		iIRCPower[*it] = port;
		this->addInPort(port);

		port = new Port(*it);
		iIRCFSPower[*it] = port;
		this->addInPort(port);

		port = new Port(*it);
		iIRCITPower[*it] = port;
		this->addInPort(port);

		port = new Port(*it);
		iIRCAvgTempCpu[*it] = port;
		this->addInPort(port);

		port = new Port(*it);
		iIRCMaxTempCpu[*it] = port;
		this->addInPort(port);

	}
	this->addInPort(&iWeatherTemp);
	sigma = 0.0;
	numData = 0;
	IRCPower = 0.0;
	pumpPower = 0.0;
	chillerPower = 0.0;
	towerPower = 0.0;
	weatherTemp = 0.0;
	IRCITPower = 0.0;
	IRCFSPower = 0.0;
	IRCAvgTempCpu = 0.0;
	IRCMaxTempCpu = 0.0;
	ircNames = puertos;
	FichSalida = salida;
}
DevsEnergyCalculator::~DevsEnergyCalculator() {

}

void DevsEnergyCalculator::initialize() {
	sigma = 0.0;
	numData = 0;
	IRCPower = 0.0;
	pumpPower = 0.0;
	chillerPower = 0.0;
	towerPower = 0.0;
	this->passivate();
}
void DevsEnergyCalculator::exit() {
	std::ofstream dataFile(FichSalida);
	//dataFile
	//		<< "\'Time\',\'Tout\',\'IT\',\'FS\',\'IRC\',\'Pump\',\'Chiller\',\'Tower\',\'Total\',\'AvgCPUTemp\',\'MaxCPUTemp\'
	for (std::map<double, tPowerData>::iterator it = PowerData.begin();
			it != PowerData.end(); ++it) {
		//'Time','Tout','IT','FS','IRC','Pump','Chiller','Tower','Total','AvgCPUTemp','MaxCPUTemp'
		if(it->second.print)
				dataFile <<(long int) it->first << ","
				<< it->second.weatherTemp << ","
				<< it->second.IRCITPower/1000 << ","
				<< it->second.IRCFSPower/1000 << ","
				<< it->second.IRCPower/1000 << ","
				<< it->second.pumpPower/1000 << ","
				<< it->second.chillerPower/1000 << ","
				<< it->second.towerPower/1000	<< ","
				<< (it->second.IRCITPower + it->second.IRCFSPower
						+ it->second.IRCPower + it->second.pumpPower
						+ it->second.chillerPower + it->second.towerPower)/1000 << ","
				<< it->second.IRCAvgTempCpu << ","
				<< it->second.IRCMaxTempCpu
				<< std::endl;
	}
	dataFile.close();
}
void DevsEnergyCalculator::deltint() {
	numData = 0;
	this->passivate();
}
void DevsEnergyCalculator::deltext(double e) {
	bool printable= false;
	if (e != 0.0) { // Se genera un nuevo log por cada momento de tiempo
		sigma += e;
	}
	numData = 0;
	if (!iPumpPower.isEmpty()) {
		Event event = iPumpPower.getSingleValue();
		pumpPower = *((double*) event.getPtr());
	}
	PowerData[sigma].pumpPower = pumpPower;

	if (!iChillerPower.isEmpty()) {
		Event event = iChillerPower.getSingleValue();
		chillerPower = *((double*) event.getPtr());
	}
	PowerData[sigma].chillerPower = chillerPower;

	if (!iTowerPower.isEmpty()) {
		Event event = iTowerPower.getSingleValue();
		towerPower = *((double*) event.getPtr());
	}
	PowerData[sigma].towerPower = towerPower;

	if (!iTowerPower.isEmpty()) {
		Event event = iWeatherTemp.getSingleValue();
		weatherTemp = *((double*) event.getPtr());
	}
	PowerData[sigma].weatherTemp = weatherTemp;

	IRCPower = 0.0;
	IRCFSPower = 0.0;
	IRCITPower = 0.0;
	IRCAvgTempCpu = 0.0;
	IRCMaxTempCpu = 0.0;
	for (std::list<std::string>::iterator itr = ircNames.begin();
			itr != ircNames.end(); ++itr) {
		if (!iIRCPower[*itr]->isEmpty()) {
			Event event = iIRCPower[*itr]->getSingleValue();
			IRCPower += *((double*) event.getPtr());
			IRCPowerMap[*itr] = *((double*) event.getPtr());
			numData++;
			printable=true;
		} else if (IRCPowerMap[*itr] != 0) {
			IRCPower += IRCPowerMap[*itr];
			numData++;
		}
		if (!iIRCFSPower[*itr]->isEmpty()) {
			Event event = iIRCFSPower[*itr]->getSingleValue();
			IRCFSPower += *((double*) event.getPtr());
			IRCFSPowerMap[*itr] = *((double*) event.getPtr());
			numData++;
			printable=true;
		} else if (IRCFSPowerMap[*itr] != 0) {
			IRCFSPower += IRCFSPowerMap[*itr];
			numData++;
		}

		if (!iIRCITPower[*itr]->isEmpty()) {
			Event event = iIRCITPower[*itr]->getSingleValue();
			IRCITPower += *((double*) event.getPtr());
			IRCITPowerMap[*itr] = *((double*) event.getPtr());
			numData++;
			printable=true;
		} else if (IRCITPowerMap[*itr] != 0) {
			IRCITPower += IRCITPowerMap[*itr];
			numData++;
		}

		if (!iIRCAvgTempCpu[*itr]->isEmpty()) {
			Event event = iIRCAvgTempCpu[*itr]->getSingleValue();
			IRCAvgTempCpu += *((double*) event.getPtr());
			IRCAvgTempCpuMap[*itr] = *((double*) event.getPtr());
			numData++;
			printable=true;
		} else if (IRCAvgTempCpuMap[*itr] != 0) {
			IRCAvgTempCpu += IRCAvgTempCpuMap[*itr];
			numData++;
		}

		if (!iIRCMaxTempCpu[*itr]->isEmpty()) {
			Event event = iIRCMaxTempCpu[*itr]->getSingleValue();
			IRCMaxTempCpu = std::max(IRCMaxTempCpu, (*((double*) event.getPtr())));
			IRCMaxTempCpuMap[*itr] = *((double*) event.getPtr());
			numData++;
			printable=true;
		} else if (IRCMaxTempCpuMap[*itr] != 0) {
			IRCMaxTempCpu = std::max(IRCMaxTempCpu,IRCMaxTempCpuMap[*itr]);
			numData++;
		}

	}
	if (numData == 5 * ircNames.size()) {
		PowerData[sigma].IRCPower = IRCPower;
		PowerData[sigma].IRCFSPower = IRCFSPower;
		PowerData[sigma].IRCITPower = IRCITPower;
		PowerData[sigma].IRCAvgTempCpu= IRCAvgTempCpu / ircNames.size();
		PowerData[sigma].IRCMaxTempCpu= IRCMaxTempCpu;
		PowerData[sigma].print= PowerData[sigma].print || printable;
		std::cout << "sigma: " << sigma
				<<" PowerData[sigma].IRCPower: " << PowerData[sigma].IRCPower
				<<" PowerData[sigma].IRCFSPower: " << PowerData[sigma].IRCFSPower
				<<" PowerData[sigma].IRCITPower: " << PowerData[sigma].IRCITPower
				<<" PowerData[sigma].IRCAvgTempCp: " << PowerData[sigma].IRCAvgTempCpu
				<<" PowerData[sigma].IRCMaxTempCpu: " << PowerData[sigma].IRCMaxTempCpu
				<< std::endl;

	}

}

void DevsEnergyCalculator::lambda() {
}

