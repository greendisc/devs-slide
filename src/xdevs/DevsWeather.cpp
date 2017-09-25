/*
 * DevsWeather.cpp
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#include "DevsWeather.h"

DevsWeather::DevsWeather(const std::string& name, const std::string& weatherFilePath)  : Atomic(name),
																						 oStop("stop"),
																						 oOut("out") {
	this->addOutPort(&oStop);
	this->addOutPort(&oOut);
	weatherFile.open(weatherFilePath);
	nextWeatherEntry = 0;
	fi=false;
	nextDiff=0.0;
}

DevsWeather::~DevsWeather() { }

void DevsWeather::initialize() {
	// We look for the first register, and store it:
	nextWeatherEntry = getNextEntry();
	if(nextWeatherEntry!=0) {
		this->holdIn("active", 0.0);
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
/*	if(fi){
		this->passivate();
	}else{
		WeatherEntry* newWeatherEntry = getNextEntry();
		if(newWeatherEntry!=0) {
			nextDiff = newWeatherEntry->second - nextWeatherEntry->second;
			if(nextDiff<0 || nextDiff>100000) {
				std::cerr << newWeatherEntry->second << " - " << nextWeatherEntry->second << " = " << diff << ". Difference is less than 0 or greater than 100000, this should not happen" << std::endl;
				fi=true;
				this->holdIn("active", 0.0);
			}
			else if(nextWeatherEntry->second==0){
				nextWeatherEntry = newWeatherEntry;
				this->holdIn("active", std::numeric_limits< double >::min());
			}
			else {
				nextWeatherEntry = newWeatherEntry;
				this->holdIn("active", nextDiff);
			}
		}
		else {
			fi=true;
			this->holdIn("active", 0.0);
		}
	}*/
	if(fi){
		this->passivate();
	}else{
		if(nextDiff<0 || nextDiff>100000) {
			std::cerr << ". Difference is less than 0 or greater than 100000, this should not happen" << std::endl;
			fi=true;
			this->holdIn("active", 0.0);
		}else{
			if(nextWeatherEntry->second == 0)
				this->holdIn("active", std::numeric_limits< double >::min());
			else
			this->holdIn("active", nextDiff);
			WeatherEntry* newWeatherEntry = getNextEntry();
			if(newWeatherEntry != 0){
				nextDiff = newWeatherEntry->second - nextWeatherEntry->second;
				nextWeatherEntry = newWeatherEntry;
			}
			else{
				fi=true;
				this->holdIn("active", 0.0);
			}
		}
	}
}

void DevsWeather::deltext(double e) {
	this->passivate();
}

void DevsWeather::lambda() {
	if(fi){
		Event event = Event::makeEvent<bool>(new bool(fi));
		oStop.addValue(event);
		std::cout << "End of execution" << std::endl;
	}else{
		Event event = Event::makeEvent<double>(new double(nextWeatherEntry->tempIncrement));
		oOut.addValue(event);
		std::cout << "Output event with time = " << nextWeatherEntry->second << ", and inc temp, = " << nextWeatherEntry->tempIncrement << std::endl;
	}
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
