/*
 * DevsIrc.cpp
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#include "DevsServer.h"

const int DevsServer::fanSpeedLUT[NUM_FAN_SPEED]={
    2200, 6000, 11000, 12200
  };
const double DevsServer::fanPowerLUT[NUM_FAN_SPEED]={
	      3.2, 17.0, 58.0, 71.82  //Real Dell model
	    };
const double DevsServer::tempCpu0LUT[NUM_FAN_SPEED][2]= {
        {40.0, 0.263}, {34.470, 0.112}, {33.877, 0.099},
        {33.354, 0.093}
    };
const double DevsServer::tempCpu1LUT[NUM_FAN_SPEED][2]={
        {40.0, 0.263}, {34.470, 0.112}, {33.877, 0.099},
        {33.354, 0.093}
    };

DevsServer::DevsServer(const std::string& name) : Atomic(name),
												  iJob("iJob"),
												  oServerPowerIT("oServerPowerIT"),
												  oServerPowerFS("oServerPowerFS"),
												  oServerAirflow("oServerAirflow"),
												  oServerTempOut("oServerTempOut"),
												  oServerAvgTempCpu("oServerAvgTempOut"),
												  oServerMaxTempCpu("oServerMaxTempCpu"){

	this->addInPort(&iJob);
	this->addOutPort(&oServerPowerIT);
	this->addOutPort(&oServerPowerFS);
	this->addOutPort(&oServerAirflow);
	this->addOutPort(&oServerTempOut);
	this->addOutPort(&oServerAvgTempCpu);
	this->addOutPort(&oServerMaxTempCpu);
	// Equivalent to initialize:
	currentJob = 0;
	cpuPower = 0.0;
	memPower = 0.0;
	cpusUsed = 0;
	//tempInlet = 18;
	cpusTemp[0] = 0;
	cpusTemp[1] = 0;
	serverPowerIT = 0.0;
	serverPowerFS = 0.0;
	serverAirflow = 0.0;
	serverTempOut = 0.0;
	tempInlet = 0.0;
	fanSpeedidx =0;
	fanSpeed = 0;
	this->passivate();
}

DevsServer::~DevsServer() {
}

void DevsServer::initialize() {
	currentJob = 0;
	cpusTemp[0] = 0;
	cpusTemp[1] = 1;
	serverPowerIT = 0.0;
	serverPowerFS = 0.0;
	serverAirflow = 0.0;
	serverTempOut = 0.0;
	updateState();
	this->holdIn("active", 0.0);
}

void DevsServer::exit() {
}

void DevsServer::deltint() {
	currentJob = 0;
	this->passivate();
}

void DevsServer::deltext(double e) {
	if(!iJob.isEmpty()) {
		Event eJob = iJob.getSingleValue();
		currentJob = new Job(*(Job*)(eJob.getPtr()));
		updateState();
		this->holdIn("active", 0.0);
	}
}

void DevsServer::lambda() {
	Event event = Event::makeEvent<double>(new double(serverPowerIT));
	oServerPowerIT.addValue(event);

	event = Event::makeEvent<double>(new double(serverPowerFS));
	oServerPowerFS.addValue(event);

	event = Event::makeEvent<double>(new double(serverAirflow));
	oServerAirflow.addValue(event);

	event = Event::makeEvent<double>(new double(serverTempOut));
	oServerTempOut.addValue(event);

	event = Event::makeEvent<double>(new double((cpusTemp[0]+cpusTemp[1])/2));
	oServerAvgTempCpu.addValue(event);

	event = Event::makeEvent<double>(new double(std::max(cpusTemp[0], cpusTemp[1])));
	oServerMaxTempCpu.addValue(event);
}

void DevsServer::updateState() {
	if(currentJob != 0){
	cpuPower = (currentJob->getBegin()==true) ? cpuPower + currentJob->getCpuPower() : cpuPower - currentJob->getCpuPower();
	memPower = (currentJob->getBegin()==true) ? memPower + currentJob->getMemPower() : memPower - currentJob->getMemPower();
	cpusUsed = (currentJob->getBegin()==true) ? cpusUsed + currentJob->getNumCores() : cpusUsed - currentJob->getNumCores();
	}
	// CPU temperature:
    cpusTemp[0] = (tempInlet - 21) + DevsServer::tempCpu0LUT[fanSpeedidx][0]+DevsServer::tempCpu0LUT[fanSpeedidx][1]*(cpuPower);
    cpusTemp[1] = (tempInlet - 21) + DevsServer::tempCpu1LUT[fanSpeedidx][0]+DevsServer::tempCpu1LUT[fanSpeedidx][1]*(cpuPower);
    // Leakage
    double leakage = -18.51 + 13.21*exp(0.008*cpusTemp[0]) -18.51 + 13.21*exp(0.008*cpusTemp[1]);
    // serverPowerIT = IDLE_POWER + memPower + cpuPower + leakage
    //const int MEM_POWER = 6.7? el cast lo deja a 6...
    serverPowerIT = 80 + memPower + cpuPower + leakage;
    serverPowerFS = fanPowerLUT[fanSpeedidx];
    serverAirflow = (0.0099*fanSpeed-13.4612)/2;
    serverTempOut = tempInlet + serverPowerIT/(serverAirflow*1.006*1.1644*1000/2118.88);
    if ((serverTempOut - tempInlet) < 5 ) {
    	serverTempOut = tempInlet + 5;
    }
    std::cout << " name: " << name << std::endl;
    std::cout << "serverAirflow: " << serverAirflow <<  "cpuPower: " << cpuPower << " memPower: "<< memPower << " cpuUsed: " << cpusUsed << " temp0: " << cpusTemp[0] << " temp1: " << cpusTemp[1] << std::endl;
    std::cout << "serverPowerIT: " << serverPowerIT << " serverPowerFS: " << serverPowerFS << " serverTempOut: " << serverTempOut << std::endl;
    std::cout << "AvgTempCp: " << ((cpusTemp[0]+cpusTemp[1])/2) << " maxTempCpu: " << std::max(cpusTemp[0], cpusTemp[1]) << std::endl;
}

void DevsServer::setInlet(int tInlet){
	this->tempInlet = tInlet;
}

void DevsServer::setInitialValues(const double values[4]){
	fanSpeedidx=getIndex((int) values[0]);
	fanSpeed = fanSpeedLUT[fanSpeedidx];
	/*    	serverFanSpeeds[rack][server][1]=numcores;
    	serverFanSpeeds[rack][server][2]=cpuPower;
    	serverFanSpeeds[rack][server][3]=memPower;*/
	cpusUsed= (int) values[1];
	cpuPower= values[2];
	memPower= values[3];
}

int DevsServer::getIndex(int fanSpeed)
{
    for (int i=0; i<NUM_FAN_SPEED; i++){
        if ( fanSpeedLUT[i] == fanSpeed){
            return i;
        }
    }
    return -1;
}
