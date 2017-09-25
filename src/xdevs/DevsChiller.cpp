/*
 * DevsChiller.cpp
 *
 *  Created on: 04/04/2017
 *      Author: roberto
 */

#include "DevsChiller.h"

const int DevsChiller::rangeLUT[NUM_RANGE_VALUES]={
		100, 86, 71, 57, 43, 29
	};
const double DevsChiller::wetBulbLUT[NUM_WETBULB_SETTINGS]{
	-1.1, 4.4, 10.0, 15.6, 21.1, 26.7, 32.2
};

const double DevsChiller::approachLUT[NUM_RANGE_VALUES][NUM_WETBULB_SETTINGS] = {
	{11.9, 8.9, 6.7, 5.0, 3.5, 2.7, 2.2},
	{10.6, 7.8, 6.0, 4.3, 3.0, 2.4, 2.1},
	{9.2, 6.9, 5.3, 3.6, 2.5, 2.0, 1.8},
	{8.3, 6.1, 4.4, 2.9, 2.2, 1.8, 1.7},
	{6.7, 4.4, 3.3, 2.2, 1.9, 1.5, 1.5},
	{5.0, 3.3, 2.2, 1.6, 1.5, 1.4, 1.4}
};


DevsChiller::DevsChiller(const std::string& name, std::list<std::string>& ircNames):Atomic(name),
iIRCGallons(), iIRCHotWaterTemp(),iWeatherTemp("iWeatherTemp"), oColdWaterTemp("oColdWaterTemp"),
oChillerPower("oChillerPower"), oTowerPower("oTowerPower"),oWeatherTemp("oWeatherTemp"),iIRCGallonsMap(),iIRCHotWaterTempMap() {

	for(std::list<std::string>::iterator itr = ircNames.begin(); itr!=ircNames.end(); ++itr) {
			Port* port = new Port(*itr);
			iIRCHotWaterTemp[*itr] = port;
			this->addInPort(port);

			port = new Port(*itr);
			iIRCGallons[*itr] = port;
			this->addInPort(port);

	}
	this->addInPort(&iWeatherTemp);
	this->addOutPort(&oColdWaterTemp);
	this->addOutPort(&oChillerPower);
	this->addOutPort(&oTowerPower);
	this->addOutPort(&oWeatherTemp);

	towerPower = 0.0;
	chillerPower = 0.0;
	weatherTemp = 0.0;
	numPowerData = 0;
	IRCGallons = 0.0;
    deltaTempAirTower = (MAXIMUM_HEAT_TOWER/KW_TON_CONVERSION)
        /(RHO_AIR*CP_AIR*1000*MAXIMUM_TOWER_AIRFLOW/CFM_CONVERSION);
    coldWaterTemp = this->FREE_COOLING_WATER_TEMP;
    accumWaterHeat = 0.0;
    converged= false;
	this->ircNames=ircNames;

}

DevsChiller::~DevsChiller() {
	//solo los de salida?
}

void DevsChiller::initialize() {
	towerPower = 0.0;
	chillerPower = 0.0;
	weatherTemp = 0.0;
	numPowerData = 0;
	IRCGallons = 0.0;
    deltaTempAirTower = (MAXIMUM_HEAT_TOWER/KW_TON_CONVERSION)
        /(RHO_AIR*CP_AIR*1000*MAXIMUM_TOWER_AIRFLOW/CFM_CONVERSION);
    coldWaterTemp = this->FREE_COOLING_WATER_TEMP;
    accumWaterHeat = 0.0;
    converged= false;
	this->holdIn("active", 0.0);
}

void DevsChiller::exit() {
}

void DevsChiller::deltint() {
	if(numPowerData==2*ircNames.size()) {
		towerPower = 0.0;
		chillerPower = 0.0;
		numPowerData = 0;
		IRCGallons = 0.0;
	    deltaTempAirTower = (MAXIMUM_HEAT_TOWER/KW_TON_CONVERSION)
	        /(RHO_AIR*CP_AIR*1000*MAXIMUM_TOWER_AIRFLOW/CFM_CONVERSION);
	    accumWaterHeat = 0.0;
	}
	this->passivate();
}

void DevsChiller::deltext(double e) {
	converged=true;
	numPowerData=0;
	accumWaterHeat=0;
	IRCGallons = 0.0;
	try {
	for(std::list<std::string>::iterator itr = ircNames.begin(); itr!=ircNames.end(); ++itr) {
		double thisGallons=0;
		double thisHotWater=0;
		if(!iIRCGallons[*itr]->isEmpty()) {
			Event event = iIRCGallons[*itr]->getSingleValue();
			IRCGallons += *((double*)event.getPtr());
			if(*((double*)event.getPtr()) < 0)
				throw ChillerException("IRC"+*itr+"couldn't reach the inlet temperature.");
			iIRCGallonsMap[*itr]=*((double*)event.getPtr());
			thisGallons=*((double*)event.getPtr());
			numPowerData++;
		}else if(iIRCGallonsMap[*itr]!=0){
			if(iIRCGallonsMap[*itr] < 0 )
				throw ChillerException("IRC "+*itr+" couldn't reach the inlet temperature.");
			IRCGallons += iIRCGallonsMap[*itr];
			thisGallons=iIRCGallonsMap[*itr];
			numPowerData++;
		}
		if(!iIRCHotWaterTemp[*itr]->isEmpty()) {
			numPowerData++;
			Event hotWaterTempEvent = iIRCHotWaterTemp[*itr]->getSingleValue();
			thisHotWater=(*((double*)hotWaterTempEvent.getPtr()));
			iIRCHotWaterTempMap[*itr]=(*((double*)hotWaterTempEvent.getPtr()));
		}else if(iIRCHotWaterTempMap[*itr] != 0){
			thisHotWater=iIRCHotWaterTempMap[*itr];
			numPowerData++;
		}
		if(thisHotWater != 0 && thisGallons != 0){
			accumWaterHeat+=thisGallons*thisHotWater;
		}
	}
	if(!iWeatherTemp.isEmpty()){
		Event eWeather = iWeatherTemp.getSingleValue();
		weatherTemp= *((double*)eWeather.getPtr());
	}
	if(numPowerData==2*ircNames.size() && weatherTemp != 0) {
		//weatherTemp=1.11111;
		accumWaterHeat/=IRCGallons;
		computePower();
			this->holdIn("active", 0.0);
	}
	}catch(ChillerException& ce){
		std::cout << ce.getException()<< std::endl;
		std::cout << "Trying to reduce temperature..." << std::endl;
		if(!reduceWaterTemp()){
			std::cout << "Impossible to reach temperature performance" << std::endl;
			this->passivate();
		}else{
			converged=false;
			this->holdIn("active", 0.0);
		}
	}
}

void DevsChiller::lambda(){
	if(!converged){
		Event event = Event::makeEvent<double>(new double(coldWaterTemp));
		oColdWaterTemp.addValue(event);
		std::cout << "Sending Tcw..." << coldWaterTemp << std::endl;
	}
	if(towerPower != 0  || chillerPower != 0){

		Event event = Event::makeEvent<double>(new double(chillerPower));
		oChillerPower.addValue(event);

		event = Event::makeEvent<double>(new double(towerPower));
		oTowerPower.addValue(event);

		event = Event::makeEvent<double>(new double(weatherTemp));
		oWeatherTemp.addValue(event);
	}
}
//Power models
void DevsChiller::computePower()
{
    // Reset variables
    chillerPower = 0; towerPower=0;
    // Compute heat
    double heat = IRCGallons*GPM_CONVERSION*RHO_WATER*CP_WATER*1000*(accumWaterHeat-coldWaterTemp);
    // See if we can use free cooling
    if (coldWaterTemp == FREE_COOLING_WATER_TEMP){
        // Try free cooling
        bool free = computeFreeCooling (heat);
        if (!free){
        	throw ChillerException("Free cooling is not enough for "+ std::to_string(heat));
        }else{
            chillerPower = 0.0;
        	return;
        }
    }
    // Use chiller
    computeChillerPower(heat);
    if(towerPower<0)
    	std::cout << "something went wrong"<< std::endl;
}

void DevsChiller::computeChillerPower(double heat)
{
    //Assume that tower extracts as much heat from water as possible
    double approach = getTowerApproach(weatherTemp+COOLING_TOWER_DESIGN_RANGE,
                                       weatherTemp, weatherTemp);

    //Compute Tct,cold == Tcondenser
    double tempCTcold = weatherTemp + approach;
    std::cout << "Temperature in heat exchanger is: " << tempCTcold
           << "( outdoor=" << weatherTemp << " approach=" << approach << ")";

    // Checking for pre-cooling
    double tempCondenser = tempCTcold;
    double tempEvap = accumWaterHeat;
    double preCooling=0;
    if ( accumWaterHeat > ( tempCTcold + HEAT_EXCHANGER_APPROACH) ){
        double deltaT = (accumWaterHeat - tempCTcold) - HEAT_EXCHANGER_APPROACH;
        tempEvap = accumWaterHeat - deltaT;
        tempCondenser = tempCTcold + deltaT;
        preCooling = RHO_WATER*CP_WATER*1000*IRCGallons*GPM_CONVERSION*deltaT;
        std::cout << "Using pre-cooling mode to decrease Tcw,hot temperature. "
               << "Deacrease of " << deltaT << "C and heat extracted: " << preCooling << std::endl;
    }

    double cop = getChillerCOP(tempEvap, tempCondenser);
    chillerPower = (heat - preCooling)/cop;
    if (chillerPower < 0){
        chillerPower = 0;
    }
    std::cout << "Chiller needs to extract " << chillerPower << " (COP= " << cop << ")"<< std::endl;
    double towerHeat = heat + chillerPower;
    std::cout << "Amount of heat in tower is: " << towerHeat<< std::endl;

    // Calculate hot water return to tower
    double tempTowerHot = tempCTcold
        + towerHeat/(RHO_WATER*CP_WATER*1000*IRCGallons*GPM_CONVERSION);
    std::cout << "Tower: hot water return temperature is " << tempTowerHot
           << "(and cold supply is: " << tempCTcold << ")";

    // Calculate tower power
    double airflowTower = towerHeat/(RHO_AIR*CP_AIR*1000*deltaTempAirTower)*CFM_CONVERSION;
    towerPower = MAXIMUM_TOWER_FAN_POWER*pow((airflowTower/MAXIMUM_TOWER_AIRFLOW), 3);
    std::cout << "Airflow for tower is: " << airflowTower << " and power: " << towerPower<< std::endl;

}

bool DevsChiller::computeFreeCooling(double heat)
{
    //Assert for approach: (number of degrees I can extract from Tct,hot)
    double approach = getTowerApproach(accumWaterHeat, coldWaterTemp,
                                       weatherTemp);
    if ( (approach+weatherTemp) > (coldWaterTemp - HEAT_EXCHANGER_APPROACH) ) {
            std::cout << "Approach assert violated: coldWaterTemp= " << coldWaterTemp
                   << " outdoorTemp= " << weatherTemp
                   << " approach= " << approach << std::endl;
        return false;
    }

    // Calculate hot water return to tower
    double tempTowerHot = accumWaterHeat;
    std::cout << "Free cooling: hot water temperature is " << tempTowerHot<< std::endl;

    // Can use free cooling.
    double airflowTower = heat/(RHO_AIR*CP_AIR*1000*deltaTempAirTower)*CFM_CONVERSION;
    towerPower = MAXIMUM_TOWER_FAN_POWER*pow((airflowTower/MAXIMUM_TOWER_AIRFLOW), 3);
    std::cout << "Free cooling can be used! Airflow for tower is: " << airflowTower
           << " and power: " << towerPower << std::endl;
    return true;
}

double DevsChiller::getTowerApproach(double hotWaterTemp, double coldWaterTemp,
                                       double wetBulbTemp)
{
    std::cout << "hotWaterTemp= " << hotWaterTemp << " coldWaterTemp= " << coldWaterTemp
           << "wetBulbTemp= " << wetBulbTemp << std::endl;
    int load; int loadIdx =0;
    if ( (hotWaterTemp - coldWaterTemp) > COOLING_TOWER_DESIGN_RANGE ){
    	std::cout << "Trying to extract too much heat from water (above design!)" << std::endl;
        load = 100;
        loadIdx = 0;
    }
    else if ( (hotWaterTemp - coldWaterTemp) == COOLING_TOWER_DESIGN_RANGE ){
    	std::cout << "Tower working at maximum capacity to extract as much heat"
                 << "from water as possible" << std::endl;
        load = 100;
        loadIdx = 0;
    }
    else {
        load = ((hotWaterTemp-coldWaterTemp)*100)/COOLING_TOWER_DESIGN_RANGE;
        // Getting load (number of degrees water heated)
        loadIdx = NUM_RANGE_VALUES - 1;
        for (int i=1; i< NUM_RANGE_VALUES; i++){
            if (load > DevsChiller::rangeLUT[i]){
                // Computing distance to load
                loadIdx = i;
                break;
            }
        }
    }
    std::cout << "Load is: " << load << std::endl;

    // Computing wetbulb
    int wetBulbIdx = computeWetBulbIdx(wetBulbTemp);

    // Getting tower approach
    double approach = approachLUT[loadIdx][wetBulbIdx];
    std::cout << "Approach is: " << approach << " (loadIdx= " << loadIdx
           << " wetBulbIdx = " << wetBulbIdx << ")" << std::endl;
    return approach;
}

bool DevsChiller::reduceWaterTemp()
{
    if (coldWaterTemp == FREE_COOLING_WATER_TEMP){
        coldWaterTemp = CHILLER_WATER_TEMP;
        return true;
    }
    return false;
}

double DevsChiller::getChillerCOP(double tempEvap, double tempCondenser)
{
    double cop;
    if (tempCondenser < tempEvap){
        //This is pre-cooling
        cop = MAX_COP_VALUE;
        std::cout << "Condenser temperature lower than evap. temperature."
               << "Chiller COP set to maximum: " << MAX_COP_VALUE<< std::endl;
        return cop;
    }
    cop = tempEvap/(tempCondenser-tempEvap);
    if (cop > MAX_COP_VALUE){
        cop=MAX_COP_VALUE;
    }
    else if(cop < MIN_COP_VALUE){
        cop=MIN_COP_VALUE;
    }
    std::cout << "Chiller COP is: " << cop << std::endl;
    return cop;
}

int DevsChiller::computeWetBulbIdx(double wetBulbTemp)
{
    if (wetBulbTemp < wetBulbLUT[0]){
        return 0;
    }
    else {
        for (int j=1; j< NUM_WETBULB_SETTINGS; j++){
            if (wetBulbTemp <= wetBulbLUT[j]){
                //Computing distance to wetbulb
                return j;
            }
        }
    }
    return 0;
}
