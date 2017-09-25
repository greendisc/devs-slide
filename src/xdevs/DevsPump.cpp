/*
 * DevsPump.cpp
 *
 *  Created on: 29 mar. 2017
 *      Author: fernando
 */

#include "DevsPump.h"

DevsPump::DevsPump(const std::string& name, std::list<std::string>& puertos):Atomic(name),oPumpPower("PumpPower"), iIRCGallons(), waterFlow(0) {

	this->addOutPort(&oPumpPower);

	for (std::list<std::string>::iterator it=puertos.begin(); it != puertos.end(); ++it){
		Port* port = new Port(*it);
		iIRCGallons[*it] = port;
		this->addInPort(port);
	}
	waterFlow = 0.0;
	IRCGallons = 0.0;
	pumpPower = 0.0;
	numIrcData = 0.0;

	IRCNames = puertos;
}

DevsPump::~DevsPump() {
}

void DevsPump::initialize(){
	this->passivate();
}
void DevsPump::exit(){

}
void DevsPump::deltint(){
	if (numIrcData != 0 ){
		numIrcData = 0;
	}
	IRCGallons = 0;
	this->passivate();
}
void DevsPump::deltext(double e){
	numIrcData=0;
	for (std::list<std::string>::iterator it=IRCNames.begin(); it != IRCNames.end(); ++it){
		if (!iIRCGallons[*it]->isEmpty()){
			Event event = iIRCGallons[*it]->getSingleValue();
			IRCGallons+= *((double*)event.getPtr());
			IRCGallonsMap[*it]=*((double*)event.getPtr());
			numIrcData++;
		}else if(IRCGallonsMap[*it] != 0){
			IRCGallons+=IRCGallonsMap[*it];
			numIrcData++;
		}
	}
	if (numIrcData == IRCNames.size()){
		computePower();
		this->holdIn("active",0.0);
	}
}
void DevsPump::lambda(){
	Event event = Event::makeEvent<double>(new double(pumpPower));
	oPumpPower.addValue(event);

}
void DevsPump::computePower(){
	double hydraPower = IRCGallons*PUMP_DELTA_PRESSURE*PUMP_FACTOR;
	pumpPower = hydraPower/PUMP_EFFICIENCY;
	    std::cout << "Pump power " << pumpPower << std::endl;

}


