/*
 * Job.h
 *
 *  Created on: 21/03/2017
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_JOB_H_
#define SRC_XDEVS_JOB_H_

#include <string>
#include <iostream>

class Job {
public:
	Job();
	Job(int id, bool begin, double startTime, double endTime, const std::string& ircName, const std::string& rackName, const std::string& serverName, int numThreads, int numCores, double cpuPower, double memPower);
	Job(const Job& src);
	Job& operator=(const Job& src);
	virtual ~Job();
	void toString();
	int getId() const { return id; }
	bool getBegin() const { return begin; }
	double getStartTime() const { return startTime; }
	void setId(int id) { this->id = id; }
	void setBegin(bool begin) { this->begin = begin; }
	void setStartTime(double startTime) { this->startTime = startTime; }

	double getCpuPower() const {
		return cpuPower;
	}

	void setCpuPower(double cpuPower) {
		this->cpuPower = cpuPower;
	}

	double getEndTime() const {
		return endTime;
	}

	void setEndTime(double endTime) {
		this->endTime = endTime;
	}

	const std::string& getIrcName() const {
		return ircName;
	}

	void setIrcName(const std::string& ircName) {
		this->ircName = ircName;
	}

	double getMemPower() const {
		return memPower;
	}

	void setMemPower(double memPower) {
		this->memPower = memPower;
	}

	int getNumCores() const {
		return numCores;
	}

	void setNumCores(int numCores) {
		this->numCores = numCores;
	}

	int getNumThreads() const {
		return numThreads;
	}

	void setNumThreads(int numThreads) {
		this->numThreads = numThreads;
	}

	const std::string& getRackName() const {
		return rackName;
	}

	void setRackName(const std::string& rackName) {
		this->rackName = rackName;
	}

	const std::string& getServerName() const {
		return serverName;
	}

	void setServerName(const std::string& serverName) {
		this->serverName = serverName;
	}

protected:
	int id;
	bool begin;
	double startTime;
    double endTime;
    std::string ircName;
    std::string rackName;
    std::string serverName;
    int numThreads;
    int numCores;
    double cpuPower;
    double memPower;
};



#endif /* SRC_XDEVS_JOB_H_ */
