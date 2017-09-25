/*
 * DevsJobsOffline.cpp
 *
 *  Created on: 14/07/2016
 *      Author: jlrisco
 */

#include "DevsJobsGenerator.h"

DevsJobsGenerator::DevsJobsGenerator(double initialTime, const std::string& name, const std::string& jobsFilePath) : Atomic(name),
																							 iStop("stop"),
																							 oOut("out") {
	this->addInPort(&iStop);
	this->addOutPort(&oOut);
	jobsFile.open(jobsFilePath);
	nextJob = 0;
	this->initialTime = initialTime;
	jobCycleCounter=0;
}

DevsJobsGenerator::~DevsJobsGenerator() {
}

void DevsJobsGenerator::initialize() {
	// We look for the first register, and store it:
	nextJob = getNextEntry();
	if(nextJob!=0) {
		this->holdIn("active", nextJob->getStartTime());
	}
	else {
		this->passivate();
	}
}

void DevsJobsGenerator::exit() {
	jobsFile.close();
}

void DevsJobsGenerator::deltint() {
	// Read next entry:
	Job* newJobEntry = getNextEntry();
	if(newJobEntry!=0) {
		double diff = newJobEntry->getStartTime() - nextJob->getStartTime();
		std::cout << "The job will be sent with time difference ... " << diff << std::endl;
		if(diff<0) {
			std::cerr << newJobEntry->getStartTime()+jobCycleCounter*initialTime << " - " << nextJob->getStartTime() +jobCycleCounter*initialTime  << " = " << diff << ". Difference is less than 0, this should not happen" << std::endl;
			this->passivate();
		}
		else {
			nextJob = newJobEntry;
			this->holdIn("active", diff);
		}
	}
	else {
		this->passivate();
	}
}

void DevsJobsGenerator::deltext(double e) {
	this->passivate();
}

void DevsJobsGenerator::lambda() {
	Event event = Event::makeEvent<Job>(nextJob);
	oOut.addValue(event);
}

Job* DevsJobsGenerator::getNextEntry() {
	Job* job = new Job();
	if(!jobsFile.eof()) {
		std::string typeAsString;
		std::string timeAsString;
		std::string idAsString;
		std::string ircAsString;
		std::string rackAsString;
		std::string serverAsString;
		std::string numThreadsAsString;
		std::string numCoresAsString;
		std::string cpuPowerAsString;
		std::string memPowerAsString;
		std::getline(jobsFile, typeAsString, ';');
		std::getline(jobsFile, timeAsString, ';');
		std::getline(jobsFile, idAsString, ';');
		std::getline(jobsFile, ircAsString, ';');
		std::getline(jobsFile, rackAsString, ';');
		std::getline(jobsFile, serverAsString, ';');
		std::getline(jobsFile, numThreadsAsString, ';');
		std::getline(jobsFile, numCoresAsString, ';');
		std::getline(jobsFile, cpuPowerAsString, ';');
		std::getline(jobsFile, memPowerAsString);
		if(typeAsString.size()>0) {
			job = new Job();
			if(typeAsString=="jobbegin") {

				job->setId(std::stoi(idAsString));
				job->setBegin(true);
				job->setStartTime(std::stol(timeAsString) - initialTime);
				job->setEndTime(0); // TODO-JOSELE: ¿Borrar EndTime? Pensarlo.
				job->setIrcName(ircAsString);
				job->setRackName(rackAsString);
				job->setServerName(serverAsString);
				job->setNumThreads(std::stoi(numThreadsAsString));
				job->setNumCores(std::stoi(numCoresAsString));
				job->setCpuPower(std::stod(cpuPowerAsString));
				job->setMemPower(std::stod(memPowerAsString));
			}
			else if(typeAsString=="jobend") {
				job->setId(std::stoi(idAsString));
				job->setBegin(false);
				job->setStartTime(std::stol(timeAsString) - initialTime);
				job->setEndTime(0);
				job->setIrcName(ircAsString);
				job->setRackName(rackAsString);
				job->setServerName(serverAsString);
				job->setNumThreads(std::stoi(numThreadsAsString));
				job->setNumCores(std::stoi(numCoresAsString));
				job->setCpuPower(std::stod(cpuPowerAsString));
				job->setMemPower(std::stod(memPowerAsString));
			}
			else if(typeAsString=="next") { // exit type
				/*job = 0; // TODO-JOSELE: A esta altura podemos volver a empezar a leer el archivo de jobs, actualizando los tiempos convenientemente.
				         // De momento lo dejo así para simplificar.*/
				jobsFile.clear();
				jobsFile.seekg(0, std::ios::beg);
				jobCycleCounter++;
				job= getNextEntry();
				nextJob->setStartTime(0);

			}
		}
	}
	else {
		std::cout << "File is OVER." << std::endl;
		job = 0;
	}
	return job;
}
