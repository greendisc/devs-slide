/*
 * Job.cpp
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#include "Job.h"

Job::Job() {
	id = -1;
	begin = false;
	startTime = 0;
	endTime = 0;
	ircName = "";
	rackName = "";
	serverName = "";
	numThreads = 0;
	numCores = 0;
	cpuPower = 0;
	memPower = 0;
}

Job::Job(int id, bool begin, double startTime, double endTime, const std::string& ircName, const std::string& rackName, const std::string& serverName, int numThreads, int numCores, double cpuPower, double memPower) {
	this->id = id;
	this->begin = begin;
	this->startTime = startTime;
	this->endTime = endTime;
	this->ircName = ircName;
	this->rackName = rackName;
	this->serverName = serverName;
	this->numThreads = numThreads;
	this->numCores = numCores;
	this->cpuPower = cpuPower;
	this->memPower = memPower;
}

Job::Job(const Job& src) {
	this->id = src.id;
	this->begin = src.begin;
	this->startTime = src.startTime;
	this->endTime = src.endTime;
	this->ircName = src.ircName;
	this->rackName = src.rackName;
	this->serverName = src.serverName;
	this->numThreads = src.numThreads;
	this->numCores = src.numCores;
	this->cpuPower = src.cpuPower;
	this->memPower = src.memPower;
}

Job& Job::operator=(const Job& src) {
	this->id = src.id;
	this->begin = src.begin;
	this->startTime = src.startTime;
	this->endTime = src.endTime;
	this->ircName = src.ircName;
	this->rackName = src.rackName;
	this->serverName = src.serverName;
	this->numThreads = src.numThreads;
	this->numCores = src.numCores;
	this->cpuPower = src.cpuPower;
	this->memPower = src.memPower;
	return (*this);
}

void Job::toString(){
	std::cout  << "printing job: "
		<<	"id: " << id
		<< "begin: ";
	if (begin)
		std::cout << "true";
	else
		std::cout << "false";
	std::cout	<< "startTime: " << startTime
		<< "endTime: " << endTime
		<< "ircName: " << ircName
		<< "rackName: "<< rackName
		<< "serverName: " << serverName
		<< "numThreads: "<< numThreads
		<< "numCores: "<< numCores
		<< "cpuPower: "<< cpuPower
		<< "memPower: "<< memPower
		<< std::endl;
}

Job::~Job() {}
