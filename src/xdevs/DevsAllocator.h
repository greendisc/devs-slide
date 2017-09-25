/*
 * DevsAllocator.h
 *
 *  Created on: 14/07/2016
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSALLOCATOR_H_
#define SRC_XDEVS_DEVSALLOCATOR_H_

// STL
#include <list>
#include <map>

// xDEVS
#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"


// OWN
#include "Job.h"

class DevsAllocator : public Atomic {
public:
	Port iJob;
	std::map<std::string, Port*> oIrcs;
	DevsAllocator(const std::string& name, std::list<std::string>& ircNames);
	virtual ~DevsAllocator();

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

protected:
	Job* currentJob;
};

#endif /* SRC_XDEVS_DEVSALLOCATOR_H_ */
