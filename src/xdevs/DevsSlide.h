/*
 * DevsSlide.h
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSSLIDE_H_
#define SRC_XDEVS_DEVSSLIDE_H_

#include <xdevs/core/modeling/Coupled.h>

#include "DevsWeather.h"

class DevsSlide : public Coupled {
protected:
	DevsWeather weather;
	DevsJobsOffline jobs;
public:
	DevsSlide(const std::string& name, const std::string& weatherFilePath, const std::string& jobsFilePath);
	virtual ~DevsSlide();
};

#endif /* SRC_XDEVS_DEVSSLIDE_H_ */
