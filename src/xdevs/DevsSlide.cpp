/*
 * DevsSlide.cpp
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#include "DevsSlide.h"

DevsSlide::DevsSlide(const std::string& name,
		             const std::string& weatherFilePath,
					 const std::string& jobsFilePath) : Coupled(name),
													    weather("Weather", weatherFilePath),
														jobs("Jobs", jobsFilePath) {
	Coupled::addComponent(&weather);
	Coupled::addComponent(&jobs);
	/*Coupled::addComponent(&processor);
	Coupled::addComponent(&transducer);
	Coupled::addCoupling(&generator, &generator.oOut, &processor, &processor.iIn);
	Coupled::addCoupling(&generator, &generator.oOut, &transducer, &transducer.iArrived);
	Coupled::addCoupling(&processor, &processor.oOut, &transducer, &transducer.iSolved);
	Coupled::addCoupling(&transducer, &transducer.oOut, &generator, &generator.iStop);*/
}

DevsSlide::~DevsSlide() { }

