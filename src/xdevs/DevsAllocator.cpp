/*
 * DevsJobsOffline.cpp
 *
 *  Created on: 14/07/2016
 *      Author: jlrisco
 */

#include "DevsAllocator.h"

DevsAllocator::DevsAllocator(const std::string& name, std::list<std::string>& ircNames) : Atomic(name),
																								iJob("iJob"),
																								oIrcs() {
	this->addInPort(&iJob);
	for(std::list<std::string>::iterator itr = ircNames.begin(); itr!=ircNames.end(); ++itr) {
		Port* port = new Port(*itr);
		oIrcs[*itr] = port;
		this->addOutPort(port);
	}
	currentJob = 0;
}

DevsAllocator::~DevsAllocator() {
	for(std::map<std::string, Port*>::iterator itr = oIrcs.begin(); itr!=oIrcs.end(); ++itr) {
		delete itr->second;
	}
}

void DevsAllocator::initialize() {
	currentJob = 0;
	this->passivate();
}

void DevsAllocator::exit() {
}

void DevsAllocator::deltint() {
	currentJob = 0;
	this->passivate();
}

void DevsAllocator::deltext(double e) {
	if(!iJob.isEmpty()) {
		Event eJob = iJob.getSingleValue();
		currentJob = new Job(*(Job*)(eJob.getPtr()));
		// TODO-JOSELE: En un futuro, aquí se debería asignar el IRC, RACK y SERVER, dependiendo de la potencia consumida y demás.
		this->holdIn("active", 0.0);
	}
}

void DevsAllocator::lambda() {
	Event event = Event::makeEvent<Job>(currentJob);
	oIrcs[currentJob->getIrcName()]->addValue(event);
}


