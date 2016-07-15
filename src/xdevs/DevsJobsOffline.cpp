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
	initialTime = 0.0;
}

DevsJobsOffline::~DevsJobsOffline() {
}

void DevsJobsOffline::initialize() {
	// We look for the first register, and store it:
	nextJobEntry = getNextEntry();
	initialTime = nextJobEntry->time;
	nextJobEntry->time = 0.0;
	if(nextJobEntry!=0) {
		this->holdIn("active", nextJobEntry->time);
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
		double diff = newJobEntry->time - nextJobEntry->time;
		if(diff<0) {
			std::cerr << newJobEntry->time << " - " << nextJobEntry->time << " = " << diff << ". Difference is less than 0, this should not happen" << std::endl;
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
	std::cout << "Output event with time = " << nextJobEntry->time << std::endl;
}

DevsJobsOffline::JobEntry* DevsJobsOffline::getNextEntry() {
	JobEntry* jobEntry = 0;
	if(!jobsFile.eof()) {
		std::string typeAsString;
		std::string timeAsString;
		std::string idAsString;
		std::getline(jobsFile, typeAsString, ';');
		std::getline(jobsFile, timeAsString, ';');
		std::getline(jobsFile, idAsString);
		if(typeAsString.size()>0) {
			jobEntry = new JobEntry;
			if(typeAsString=="jobbegin") {
				jobEntry->begin = true;
				jobEntry->time = std::stol(timeAsString) - initialTime;
				jobEntry->id = std::stoi(idAsString);
			}
			else if(typeAsString=="jobend") {
				jobEntry->begin = false;
				jobEntry->time = std::stol(timeAsString) - initialTime;
				jobEntry->id = std::stoi(idAsString);
			}
			else { // exit type
				jobEntry->begin = false;
				jobEntry->time = Constants::INFINITY;
				jobEntry->id = 0;
			}
		}
	}
	else {
		std::cout << "File is OVER." << std::endl;
	}
	return jobEntry;
}
