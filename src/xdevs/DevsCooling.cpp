/*
 * DevsCooling.cpp
 *
 *  Created on: 26/04/2017
 *      Author: roberto
 */

#include "DevsCooling.h"

DevsCooling::DevsCooling(std::list<std::string>& ircNames):Coupled("DevsCooling"),oPumpPower("PumpPower") , iWeatherTemp("iWeatherTemp"), oColdWaterTemp("oColdWaterTemp"),
oChillerPower("oChillerPower"), oTowerPower("oTowerPower"),oWeatherTemp("oWeatherTemp"){
	DevsPump* pump= new DevsPump("Pump", ircNames);
	Coupled::addComponent(pump);
	this->addOutPort(&oPumpPower);
	Coupled::addCoupling(pump, &pump->oPumpPower, this, &this->oPumpPower);
	for (std::list<std::string>::iterator it=ircNames.begin(); it != ircNames.end(); ++it){
		Port* port = new Port(*it);
		iIRCGallons[*it] = port;
		this->addInPort(port);
		Coupled::addCoupling(this, this->iIRCGallons[*it], pump, pump->iIRCGallons[*it]);
	}
	//Chiller
	DevsChiller* chiller =new DevsChiller("Chiller", ircNames);
	Coupled::addComponent(chiller);

	for(std::list<std::string>::iterator itr = ircNames.begin(); itr!=ircNames.end(); ++itr) {
			Port* port = new Port(*itr);
			iIRCHotWaterTemp[*itr] = port;
			this->addInPort(port);
			Coupled::addCoupling(this, this->iIRCHotWaterTemp[*itr], chiller , chiller->iIRCHotWaterTemp[*itr]);
			Coupled::addCoupling(this, this->iIRCGallons[*itr], chiller, chiller->iIRCGallons[*itr]);
	}
	this->addInPort(&iWeatherTemp);
	Coupled::addCoupling(this, &this->iWeatherTemp, chiller, &chiller->iWeatherTemp);
	this->addOutPort(&oColdWaterTemp);
	Coupled::addCoupling(chiller, &chiller->oColdWaterTemp,this, &this->oColdWaterTemp );
	this->addOutPort(&oChillerPower);
	Coupled::addCoupling(chiller, &chiller->oChillerPower, this, &this->oChillerPower);
	this->addOutPort(&oTowerPower);
	Coupled::addCoupling(chiller, &chiller->oTowerPower, this, &this->oTowerPower);
	this->addOutPort(&oWeatherTemp);
	Coupled::addCoupling(chiller, &chiller->oWeatherTemp, this, &this->oWeatherTemp);


}

DevsCooling::~DevsCooling() {
	std::list<Component*> components = this->getComponents();
	for(std::list<Component*>::iterator itr = components.begin(); itr!=components.end(); ++itr) {
		delete *itr;
	}
}

