/*
 * DevsIrc.cpp
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#include "DevsRack.h"

DevsRack::DevsRack(const std::string& name, std::list<std::string>& serverNames) : Atomic(name),
																			   	   iJob("iJob"),
																				   ports(),
																				   oJob(),
																				   oRackPowerIT("oRackPowerIT"),
																				   oRackPowerFS("oRackPowerFS"),
																				   oRackAirflow("oRackAirflow"),
																				   oRackTempOut("oRackTempOut"),
																				   oRackAvgTempCpu("oRackAvgTempCpu"),
																				   oRackMaxTempCpu("oRackMaxTempCpu"),
																				   serverNames(serverNames)
																				   {
	this->addInPort(&iJob);
	for(std::list<std::string>::iterator itr = serverNames.begin(); itr!=serverNames.end(); ++itr) {
		Port* port = new Port(*itr);
		oJob[*itr] = port;
		this->addOutPort(port);

		port = new Port(*itr);
		ports[*itr].iServersPowerIT = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersAirflow = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersPowerFS = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersAirflow = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersTempOut = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersAvgTempCpu = port;
		this->addInPort(port);

		port = new Port(*itr);
		ports[*itr].iServersMaxTempCpu = port;
		this->addInPort(port);
	}
	this->addOutPort(&oRackPowerIT);
	this->addOutPort(&oRackPowerFS);
	this->addOutPort(&oRackAirflow);
	this->addOutPort(&oRackTempOut);
	this->addOutPort(&oRackAvgTempCpu);
	this->addOutPort(&oRackMaxTempCpu);

	currentJob = 0;
	serversPowerIT = 0.0;
	serversPowerFS = 0.0;
	serversAirflow = 0.0;
	serversTempOut = 0.0;
	serversMaxTempCpu =0.0;
	serversAvgTempCpu = 0.0;
	numPowerData = 0;
}

DevsRack::~DevsRack() {
	for(std::map<std::string, Port*>::iterator itr = oJob.begin(); itr!=oJob.end(); ++itr) {
		delete itr->second;
	}
// Solo se destruyen los puertos de salida, porque el acoplado devs-slide borra el resto de puertos.
}

void DevsRack::initialize() {
	currentJob = 0;
	serversPowerIT = 0;
	serversPowerFS = 0;
	serversAirflow = 0;
	serversTempOut = 0;
	numPowerData = 0;
	serversMaxTempCpu =0.0;
	serversAvgTempCpu = 0.0;
	this->passivate();
}

void DevsRack::exit() {
}

void DevsRack::deltint() {
	if(currentJob!=0) {
		currentJob = 0;
	}
	if(numPowerData==6*serverNames.size()) {
		serversPowerIT = 0;
		serversPowerFS = 0;
		serversAirflow = 0;
		serversTempOut = 0;
		numPowerData = 0;
	}
	this->passivate();
}

void DevsRack::deltext(double e) {
	if(!iJob.isEmpty()) {
		Event eJob = iJob.getSingleValue();
		currentJob = new Job(*(Job*)(eJob.getPtr()));
		this->holdIn("active", 0.0);
	}
	serversPowerIT = 0.0;
	serversPowerFS = 0.0;
	serversAirflow = 0.0;
	serversTempOut = 0.0;
	serversMaxTempCpu = 0.0;
	serversAvgTempCpu = 0.0;
	numPowerData = 0;
	for(std::list<std::string>::iterator itr = serverNames.begin(); itr!=serverNames.end(); ++itr) {
		double thisAirflow= 0.0;
		double thisTempsOut= 0.0;
		tPorts* targetPort= &(ports[*itr]);
		if(!targetPort->iServersPowerIT->isEmpty()) {
			Event event = targetPort->iServersPowerIT->getSingleValue();
			serversPowerIT += *((double*)event.getPtr());
			targetPort->iServersPowerITValue=*((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersPowerITValue >= 0){
			serversPowerIT+= targetPort->iServersPowerITValue;
			numPowerData++;
		}
		if(!targetPort->iServersPowerFS->isEmpty()) {
			Event event = targetPort->iServersPowerFS->getSingleValue();
			serversPowerFS += *((double*)event.getPtr());
			targetPort->iServersPowerFSValue=*((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersPowerFSValue >= 0){
			serversPowerFS+= targetPort->iServersPowerFSValue;
			numPowerData++;
		}
		if(!targetPort->iServersAirflow->isEmpty()) {
			Event event = targetPort->iServersAirflow->getSingleValue();
			serversAirflow += *((double*)event.getPtr());
			targetPort->iServersAirflowValue=*((double*)event.getPtr());
			thisAirflow= *((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersAirflowValue >= 0){
			serversAirflow+= targetPort->iServersAirflowValue;
			thisAirflow = targetPort->iServersAirflowValue;
			numPowerData++;
		}
		if(!targetPort->iServersTempOut->isEmpty()) {
			Event event = targetPort->iServersTempOut->getSingleValue();
			targetPort->iServersTempOutValue=*((double*)event.getPtr());
			thisTempsOut=*((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersTempOutValue != std::numeric_limits<double>::min()){
			thisTempsOut=targetPort->iServersTempOutValue;
			numPowerData++;
		}
		if(thisAirflow != 0 && thisTempsOut != 0){
			serversTempOut+=thisAirflow * thisTempsOut;
		}
		if(!targetPort->iServersAvgTempCpu->isEmpty()) {
			Event event = targetPort->iServersAvgTempCpu->getSingleValue();
			serversAvgTempCpu+=*((double*)event.getPtr());
			targetPort->iServersAvgTempCpuValue=*((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersAvgTempCpuValue != std::numeric_limits<double>::min()){
			serversAvgTempCpu+=targetPort->iServersAvgTempCpuValue;
			numPowerData++;
		}

		if(!targetPort->iServersMaxTempCpu->isEmpty()) {
			Event event = targetPort->iServersMaxTempCpu->getSingleValue();
			serversMaxTempCpu=std::max(serversMaxTempCpu, (*((double*)event.getPtr())));
			targetPort->iServersMaxTempCpuValue=*((double*)event.getPtr());
			numPowerData++;
		}else if(targetPort->iServersMaxTempCpuValue != std::numeric_limits<double>::min()){
			serversMaxTempCpu=std::max(serversMaxTempCpu,targetPort->iServersMaxTempCpuValue);
			numPowerData++;
		}
	}
	if(numPowerData==6*serverNames.size()) {
		serversAvgTempCpu/=serverNames.size();
		std::cout << "name: "<< name
			<< " serversPowerIT: " << serversPowerIT
			<< " serversPowerFS: " << serversPowerFS
			<< " serversAirflow: " << serversAirflow
			<< " serversTempOut: " << serversTempOut
			<< " numPowerData: " << numPowerData
			<< " serversMaxTempCpu: " <<  serversMaxTempCpu
			<< " serversAvgTempCpu: " << serversAvgTempCpu
			<< std::endl;
		this->holdIn("active", 0.0);
	}
}

void DevsRack::lambda() {
	if(currentJob!=0) {
		Event event = Event::makeEvent<Job>(currentJob);
		oJob[currentJob->getServerName()]->addValue(event);
	}
	if(numPowerData==6*serverNames.size()) {
		Event event = Event::makeEvent<double>(new double(serversPowerIT));
		oRackPowerIT.addValue(event);
		event = Event::makeEvent<double>(new double(serversPowerFS));
		oRackPowerFS.addValue(event);
		event = Event::makeEvent<double>(new double(serversAirflow));
		oRackAirflow.addValue(event);
		event = Event::makeEvent<double>(new double(serversTempOut));
		oRackTempOut.addValue(event);
		event = Event::makeEvent<double>(new double(serversAvgTempCpu));
		oRackAvgTempCpu.addValue(event);
		event = Event::makeEvent<double>(new double(serversMaxTempCpu));
		oRackMaxTempCpu.addValue(event);

	}
}
