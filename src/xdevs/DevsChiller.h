/*
 * DevsChiller.h
 *
 *  Created on: 04/04/2017
 *      Author: roberto
 */

#ifndef SRC_XDEVS_DEVSCHILLER_H_
#define SRC_XDEVS_DEVSCHILLER_H_
//STL
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <math.h>

//xDEVS
#include "../../lib/xdevs/xdevs/core/modeling/Port.h"
#include "../../lib/xdevs/xdevs/core/modeling/Atomic.h"
#include "../../lib/xdevs/xdevs/core/modeling/Event.h"


// Constant definition
#define    CP_AIR       1.006      // (KJ/KgC)
#define    RHO_AIR      1.1644     // (kg/m3)

#define    CP_WATER     4.187      // (KJ/KgC)
#define    RHO_WATER    999.3      // (kg/m3)

#define    CFM_CONVERSION          2118.88
#define    GPM_CONVERSION          0.00006389
#define    KW_TON_CONVERSION       0.000284345
#define    HP_TO_WATT_CONVERSION   745.7

class DevsChiller: public Atomic {
public:
	class ChillerException{
	public:
		ChillerException(const std::string& s){
			e=s;
		}
		std::string getException(){
			return e;
		}
	private:
		std::string e;
	};
	DevsChiller(const std::string& name, std::list<std::string>& IrcNames);
	virtual ~DevsChiller();


  // Model LUT's
	static constexpr double FREE_COOLING_WATER_TEMP = 18.3;
	static constexpr double CHILLER_WATER_TEMP = 12.7;

	// Chiller params
	static constexpr int MAX_COP_VALUE = 8;
	static constexpr double MIN_COP_VALUE = 2;

	// Heat exchanger
	static constexpr double HEAT_EXCHANGER_APPROACH = 1.1;

	//Tower params
	static constexpr double COOLING_TOWER_DESIGN_RANGE = 15;  //celsius
	static constexpr double COOLING_TOWER_DESIGN_APPROACH = 2.5;  //celsius

	static constexpr double MAXIMUM_HEAT_TOWER = 85 ;         //ton
	static constexpr double COOLING_TOWER_WATERFLOW = 3*MAXIMUM_HEAT_TOWER ;    //gpm
	static constexpr double MAXIMUM_TOWER_FAN_POWER =
		0.2*MAXIMUM_HEAT_TOWER*HP_TO_WATT_CONVERSION ;  //Watt
	static constexpr double MAXIMUM_TOWER_AIRFLOW =
		1000*MAXIMUM_TOWER_FAN_POWER/HP_TO_WATT_CONVERSION ;    //cfm

	// Tower approach
	static const int NUM_RANGE_VALUES = 6;
	static const int NUM_WETBULB_SETTINGS = 7;

	static const int rangeLUT[NUM_RANGE_VALUES];
	static const double wetBulbLUT[NUM_WETBULB_SETTINGS];
	static const double approachLUT[NUM_RANGE_VALUES][NUM_WETBULB_SETTINGS];


	std::map<std::string, Port*> iIRCGallons;
	std::map<std::string, Port*> iIRCHotWaterTemp;
	Port iWeatherTemp;

	Port oColdWaterTemp;
	Port oChillerPower;
	Port oTowerPower;
	Port oWeatherTemp;

	// DEVS protocol
	virtual void initialize();
	virtual void exit();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void lambda();

	//OWN
	bool reduceWaterTemp();
	void computePower();
	bool computeFreeCooling(double heat);
	void computeChillerPower(double heat);
	double getTowerApproach(double hotWaterTemp, double coldWaterTemp,
	                                       double wetBulbTemp);
	int computeWetBulbIdx(double wetBulbTemp);
	double getChillerCOP(double tempEvap, double tempCondenser);
protected:
	std::map<std::string, double> iIRCGallonsMap;
	std::map<std::string, double> iIRCHotWaterTempMap;
	double towerPower;
	double chillerPower;
	double weatherTemp;
	double IRCGallons;
	double deltaTempAirTower;
	double coldWaterTemp;
	double accumWaterHeat;
	unsigned int numPowerData;
	bool converged;
	std::list<std::string> ircNames;

};

#endif /* SRC_XDEVS_DEVSCHILLER_H_ */
