/*
 * DevsJobsOffline.h
 *
 *  Created on: 14/07/2016
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSJOBSGENERATOR_H_
#define SRC_XDEVS_DEVSJOBSGENERATOR_H_

#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"
#include <fstream>
#include <iostream>

#include "Job.h"

class DevsJobsGenerator : public Atomic {
public:
	Port iStop;
	Port oOut;
	DevsJobsGenerator(double initialTime, const std::string& name, const std::string& jobsFilePath);
	virtual ~DevsJobsGenerator();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

	// PARTICULAR FUNCTIONS
	Job* getNextEntry();
protected:
	double initialTime;
	std::ifstream jobsFile;
	unsigned int jobCycleCounter;
	Job* nextJob;
};

#endif /* SRC_XDEVS_DEVSJOBSGENERATOR_H_ */
