/*
 * DevsJobsOffline.cpp
 *
 *  Created on: 14/07/2016
 *      Author: jlrisco
 */

#include "DevsJobsOffline.h"

DevsJobsOffline::DevsJobsOffline(const std::string& name, const std::string& jobsFilePath) : Atomic(name),
																							 iStop("stop"),
																							 oOut("out") {
	this->addInPort(&iStop);
	this->addOutPort(&oOut);
	jobsFile.open(jobsFilePath);
	nextJobEntry = 0;
}

DevsJobsOffline::~DevsJobsOffline() {
}

void DevsJobsOffline::initialize() {
	// We look for the first register, and store it:
	nextJobEntry = getNextEntry();
	if(nextJobEntry!=0) {
		this->holdIn("active", nextJobEntry->second);
	}
	else {
		this->passivate();
	}
}

void DevsJobsOffline::exit() {
	jobsFile.close();
}

void DevsJobsOffline::deltint() {
	// Read next entry:
	JobEntry* newJobEntry = getNextEntry();
	if(newJobEntry!=0) {
		long diff = newJobEntry->second - nextJobEntry->second;
		if(diff<0 || diff>100000) {
			std::cerr << newJobEntry->second << " - " << nextJobEntry->second << " = " << diff << ". Difference is less than 0 or greater than 100000, this should not happen" << std::endl;
			this->passivate();
		}
		else {
			nextJobEntry = newJobEntry;
			this->holdIn("active", diff);
		}
	}
	else {
		this->passivate();
	}
}

void DevsJobsOffline::deltext(double e) {
	this->passivate();
}

void DevsJobsOffline::lambda() {
	Event event = Event::makeEvent<JobEntry>(nextJobEntry);
	oOut.addValue(event);
	std::cout << "Output event with time = " << nextJobEntry->second << ", and inc temp, = " << nextJobEntry->tempIncrement << std::endl;
}

DevsJobsOffline::JobEntry* DevsJobsOffline::getNextEntry() {
	JobEntry* weatherEntry = 0;
	if(!jobsFile.eof()) {
		std::string secondAsString;
		std::string tempIncrementAsString;
		std::getline(jobsFile, secondAsString, ',');
		std::getline(jobsFile, tempIncrementAsString);
		if(secondAsString.size()>0 && tempIncrementAsString.size()>0) {
			weatherEntry = new JobEntry;
			weatherEntry->second = std::stoi(secondAsString);
			weatherEntry->tempIncrement = std::stod(tempIncrementAsString);
		}
	}
	else {
		std::cout << "File is OVER." << std::endl;
	}
	return weatherEntry;
}
