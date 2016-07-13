/*
 * DevsWeather.cpp
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#include "DevsWeather.h"

DevsWeather::DevsWeather(const std::string& name, const std::string& weatherFilePath)  : Atomic(name),
																						 iStop("stop"),
																						 oOut("out") {
	this->addInPort(&iStop);
	this->addOutPort(&oOut);
	weatherFile.open(weatherFilePath);
	nextWeatherEntry = 0;
}

DevsWeather::~DevsWeather() { }

void DevsWeather::initialize() {
	// We look for the first register, and store it:
	nextWeatherEntry = getNextEntry();
	if(nextWeatherEntry!=0) {
		this->holdIn("active", nextWeatherEntry->second);
	}
	else {
		this->passivate();
	}
}

void DevsWeather::exit() {
	weatherFile.close();
}

void DevsWeather::deltint() {
	// Read next entry:
	WeatherEntry* newWeatherEntry = getNextEntry();
	if(newWeatherEntry!=0) {
		long diff = newWeatherEntry->second - nextWeatherEntry->second;
		if(diff<0 || diff>100000) {
			std::cerr << newWeatherEntry->second << " - " << nextWeatherEntry->second << " = " << diff << ". Difference is less than 0 or greater than 100000, this should not happen" << std::endl;
			this->passivate();
		}
		else {
			nextWeatherEntry = newWeatherEntry;
			this->holdIn("active", diff);
		}
	}
	else {
		this->passivate();
	}
}

void DevsWeather::deltext(double e) {
	this->passivate();
}

void DevsWeather::lambda() {
	Event event = Event::makeEvent<WeatherEntry>(nextWeatherEntry);
	oOut.addValue(event);
	std::cout << "Output event with time = " << nextWeatherEntry->second << ", and inc temp, = " << nextWeatherEntry->tempIncrement << std::endl;
}

DevsWeather::WeatherEntry* DevsWeather::getNextEntry() {
	WeatherEntry* weatherEntry = 0;
	if(!weatherFile.eof()) {
		std::string secondAsString;
		std::string tempIncrementAsString;
		std::getline(weatherFile, secondAsString, ',');
		std::getline(weatherFile, tempIncrementAsString);
		if(secondAsString.size()>0 && tempIncrementAsString.size()>0) {
			weatherEntry = new WeatherEntry;
			weatherEntry->second = std::stoi(secondAsString);
			weatherEntry->tempIncrement = std::stod(tempIncrementAsString);
		}
	}
	else {
		std::cout << "File is OVER." << std::endl;
	}
	return weatherEntry;
}
