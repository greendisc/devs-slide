/*
 * DevsIrc.cpp
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

// En la gestión de puertos de las clases que produzcan transiciones externas a través de múltiples puertos
// deben tener todos los valores antes de producir una lambda. Por tanto, hay una variable numData que controla
// la cantidad de datos recibidos.

#include "DevsIrc.h"

const double DevsIrc::perfLUT[NUM_AIRFLOW_VALUES][NUM_GALLONS_SETTING] =
	{
		{ 437.5, 560, 560, 560, 560 },
		{ 562.5, 748.75, 780, 805, 811.25 },
		{ 687.5, 937.5, 1000, 1050, 1062.5 },
		{ 750, 1062.5, 1187.5, 1243.75, 1281.25 },
		{ 812.5, 1187.5, 1375, 1437.5, 1500 },
		{ 843.75, 1281.25, 1500, 1625, 1687.5 },
		{ 875, 1375, 1625, 1812.5, 1875 },
		{ 885.415, 1437.5, 1718.75, 1937.5, 2031.25 },
		{ 895.83, 1500, 1812.5, 2062.5, 2187.5 },
		{ 906.255, 1535, 1906.25, 2156.25, 2306.25 },
		{ 916.68, 1570, 2000, 2250, 2425 },
		{ 927.09, 1610, 2062.5, 2312.5, 2525 },
		{ 937.5, 1650, 2125, 2375, 2625 } };

const int DevsIrc::gallonsLUT[NUM_GALLONS_SETTING] =
	{ 5, 10, 15, 20, 25 };

const int DevsIrc::airflowLUT[NUM_AIRFLOW_VALUES] =
	{ 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500,
			7000 };

DevsIrc::DevsIrc(const std::string& name, std::list<std::string>& rackNames)
		: Atomic(name), iJob("iJob"), iWaterTemp("iWaterTemp"), iRacksPowerIT(), iRacksPowerFS(), iRacksAirflow(), iRacksTempOut(), iRacksAvgTempCpu(), iRacksMaxTempCpu(), oJob(),
		  	  	  oIRCGallons("oIRCGallons"), oIRCPower("oIRCPower"), oIRCITPower("oIRCITPower"),
				  oIRCFSPower("oIRCFSPower"), oIRCHotWater("oIRCHotWater"), oIRCAvgTempCpu("oIRCAvgTempCpu"),
				  oIRCMaxTempCpu("oIRCMaxTempCpu"), rackNames(rackNames), iRacksPowerITMap(), iRacksPowerFSMap(),
				  iRacksAirflowMap(), iRacksTempOutMap() {

	this->addInPort(&iWaterTemp);
	this->addInPort(&iJob);
	//Gestión de puertos: para generar x puertos por cada atomico
	//es necesario iterar sobre una lista de racks en este caso generando dinamicamente los puertos
	for (std::list<std::string>::iterator itr = rackNames.begin();
			itr != rackNames.end(); ++itr) {
		Port* port = new Port(*itr);
		oJob[*itr] = port;
		this->addOutPort(port);

		port = new Port(*itr);
		iRacksPowerIT[*itr] = port;
		this->addInPort(port);

		port = new Port(*itr);
		iRacksPowerFS[*itr] = port;
		this->addInPort(port);

		port = new Port(*itr);
		iRacksAirflow[*itr] = port;
		this->addInPort(port);

		port = new Port(*itr);
		iRacksTempOut[*itr] = port;
		this->addInPort(port);

		port = new Port(*itr);
		iRacksAvgTempCpu[*itr] = port;
		this->addInPort(port);

		port = new Port(*itr);
		iRacksMaxTempCpu[*itr] = port;
		this->addInPort(port);
	}
	this->addOutPort(&oIRCGallons);
	this->addOutPort(&oIRCPower);
	this->addOutPort(&oIRCHotWater);
	this->addOutPort(&oIRCITPower);
	this->addOutPort(&oIRCFSPower);
	this->addOutPort(&oIRCAvgTempCpu);
	this->addOutPort(&oIRCMaxTempCpu);

	currentJob = 0;
	racksPowerIT = 0;
	racksPowerFS = 0;
	racksAirflow = 0;
	racksTempOut = 0;
	racksAvgTout = 0;
	IRCGallons = 0;
	IRCPower = 0;
	coolerWaterTemp = 0;
	IRCMaxTempCpu =0;
	IRCAvgTempCpu= 0;
	numPowerData = 0;
	deltaHotWater = 0;


}

DevsIrc::~DevsIrc() {
	for (std::map<std::string, Port*>::iterator itr = oJob.begin();
			itr != oJob.end(); ++itr) {
		delete itr->second;
	}
}

void DevsIrc::initialize() {
	currentJob = 0;
	racksPowerIT = 0;
	racksPowerFS = 0;
	racksAirflow = 0;
	racksTempOut = 0;
	numPowerData = 0;
	this->passivate();
}

void DevsIrc::exit() {
}

void DevsIrc::deltint() {
	if (currentJob != 0) {
		currentJob = 0;
	}
	if (numPowerData == 6 * rackNames.size()) {
		racksPowerIT = 0;
		racksPowerFS = 0;
		racksAirflow = 0;
		racksTempOut = 0;
		racksAvgTout = 0;
		IRCGallons = 0;
		IRCPower = 0;
		deltaHotWater = 0;
		numPowerData = 0;

	}
	this->passivate();
}

void DevsIrc::deltext(double e) {
	if (!iJob.isEmpty()) {
		Event eJob = iJob.getSingleValue();
		currentJob = new Job(*(Job*) (eJob.getPtr()));
		this->holdIn("active", 0.0);
	}
	racksPowerIT = 0.0;
	racksPowerFS = 0.0;
	racksAirflow = 0.0;
	racksTempOut = 0.0;
	racksAvgTout = 0.0;
	IRCAvgTempCpu= 0.0;
	IRCMaxTempCpu = 0.0;
	IRCGallons = 0.0;
	IRCPower = 0.0;
	deltaHotWater = 0.0;
	numPowerData = 0;
	bool newDataFromRacks=false;
	//Si un dato no ha cambiado se mira el mapa de valores,
	//si este tampoco tiene el dato se considera que no se puede cambiar el estado.
	//Este comportamiento se mantiene en todos los atomicos del modelo.
	for (std::list<std::string>::iterator itr = rackNames.begin();
			itr != rackNames.end(); ++itr) {
		if (!iRacksPowerIT[*itr]->isEmpty()) {
			Event event = iRacksPowerIT[*itr]->getSingleValue();
			racksPowerIT += *((double*) event.getPtr());
			iRacksPowerITMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksPowerITMap[*itr] != 0) {
			racksPowerIT += iRacksPowerITMap[*itr];
			numPowerData++;
		}
		if (!iRacksPowerFS[*itr]->isEmpty()) {
			Event event = iRacksPowerFS[*itr]->getSingleValue();
			racksPowerFS += *((double*) event.getPtr());
			iRacksPowerFSMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksPowerFSMap[*itr] != 0) {
			racksPowerFS += iRacksPowerFSMap[*itr];
			numPowerData++;
		}
		if (!iRacksAirflow[*itr]->isEmpty()) {
			Event event = iRacksAirflow[*itr]->getSingleValue();
			racksAirflow += *((double*) event.getPtr());
			iRacksAirflowMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksAirflowMap[*itr] != 0) {
			racksAirflow += iRacksAirflowMap[*itr];
			numPowerData++;
		}
		if (!iRacksTempOut[*itr]->isEmpty()) {
			Event event = iRacksTempOut[*itr]->getSingleValue();
			racksTempOut += *((double*) event.getPtr());
			iRacksTempOutMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksTempOutMap[*itr] != 0) {
			racksTempOut += iRacksTempOutMap[*itr];
			numPowerData++;
		}
		if (!iRacksAvgTempCpu[*itr]->isEmpty()) {
			Event event = iRacksAvgTempCpu[*itr]->getSingleValue();
			IRCAvgTempCpu += *((double*) event.getPtr());
			iRacksAvgTempCpuMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksAvgTempCpuMap[*itr] != 0) {
			IRCAvgTempCpu += iRacksAvgTempCpuMap[*itr];
			numPowerData++;
		}
		if (!iRacksMaxTempCpu[*itr]->isEmpty()) {
			Event event = iRacksMaxTempCpu[*itr]->getSingleValue();
			IRCMaxTempCpu =std::max(IRCMaxTempCpu, (*((double*) event.getPtr())));
			iRacksMaxTempCpuMap[*itr] = *((double*) event.getPtr());
			numPowerData++;
			newDataFromRacks=true;
		} else if (iRacksMaxTempCpuMap[*itr] != 0) {
			IRCMaxTempCpu =std::max(IRCMaxTempCpu, iRacksMaxTempCpuMap[*itr]);
			numPowerData++;
		}

	}
	if (!this->iWaterTemp.isEmpty()) {
		Event event = iWaterTemp.getSingleValue();
		coolerWaterTemp = *((double*) event.getPtr());
		newDataFromRacks=true;
	}
	if (numPowerData == 6 * rackNames.size() && coolerWaterTemp != 0 && newDataFromRacks) {
		IRCAvgTempCpu/=rackNames.size();
		racksAvgTout = racksTempOut / racksAirflow;
		IRCGallons = computeGallons();	//falta el compute HotWater
		IRCPower = computePower();
		computeHotWaterTemp();
		// Solo se modifica el estado si le llegan nuevos datos de los racks, descartando los trabajos.
			std::cout << "name: " << name
					<< " coolerWaterTemp: "	<< coolerWaterTemp
					<< " racksPowerIT: " << racksPowerIT
					<< " racksPowerFS: " << racksPowerFS
					<< " racksAirflow: " << racksAirflow << " racksTempOut: " << racksTempOut
					<< " numPowerData: " << numPowerData
					<< " racksAvgTout: " << racksAvgTout
					<< " IRCGallons: " 	<< IRCGallons
					<< " IRCPower: " << IRCPower
					<< " deltaHotWater: " << deltaHotWater
					<< std::endl;
			std::cout <<" IRCAvgTempCpu: "<< IRCAvgTempCpu
					<<" IRCMaxTempCpu: "<< IRCMaxTempCpu
					<< std::endl;
			this->holdIn("active", 0.0);
	}

}

void DevsIrc::lambda() {
	if (currentJob != 0) {
		Event event = Event::makeEvent<Job>(currentJob);
		oJob[currentJob->getRackName()]->addValue(event);
	}
	if (IRCGallons != 0 && IRCPower != 0) {
		Event event = Event::makeEvent<double>(new double(IRCGallons));
		oIRCGallons.addValue(event);
		oldValues[1] = IRCGallons;

		event = Event::makeEvent<double>(new double(IRCPower));
		oIRCPower.addValue(event);
		oldValues[2] = IRCPower;

		event = Event::makeEvent<double>(new double(deltaHotWater));
		oIRCHotWater.addValue(event);
		oldValues[3] = deltaHotWater;

		event = Event::makeEvent<double>(new double(racksPowerFS));
		oIRCFSPower.addValue(event);

		event = Event::makeEvent<double>(new double(racksPowerIT));
		oIRCITPower.addValue(event);

		event = Event::makeEvent<double>(new double(IRCAvgTempCpu));
		oIRCAvgTempCpu.addValue(event);

		event = Event::makeEvent<double>(new double(IRCMaxTempCpu));
		oIRCMaxTempCpu.addValue(event);
	}
}

double DevsIrc::computeGallons() {
	int idx = getAirflowIdx();
	//coolerWaterTemp = tempWaterCold; viene del puerto de chiller
	double perf = racksPowerIT / (racksAvgTout - coolerWaterTemp);
	for (int i = 0; i < NUM_GALLONS_SETTING; i++) {
		if (perfLUT[idx][i] > perf) {
			return gallonsLUT[i];
		}
	}
	std::cout << "Performance " << perf << " cannot be met with airflow "
			<< racksAirflow << std::endl;
	return -1;//si alguno es negativo hay que reducir la temperatura del agua...
}

double DevsIrc::computePower() {
	int idx = getAirflowIdx();
	double realflow = airflowLUT[idx];

	double power = 0.000000015658 * pow(realflow, 3)
			+ 0.000030196 * pow(realflow, 2) - 0.0317 * realflow + 198.9612;

	return power;
}

void DevsIrc::computeHotWaterTemp()	//incremento de temp del agua
{
	double delta = racksPowerIT
			/ (IRCGallons * GPM_CONVERSION * RHO_WATER * CP_WATER * 1000);
	deltaHotWater = (coolerWaterTemp + delta);
}

int DevsIrc::getAirflowIdx() {
	for (int i = 0; i < NUM_AIRFLOW_VALUES; i++) {
		if (airflowLUT[i] > racksAirflow) {
			return i;
		}
	}
	return -1;
}
