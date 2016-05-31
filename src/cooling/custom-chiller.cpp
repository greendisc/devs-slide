/*
 * ===================================================================
 *
 *       Filename:  custom-chiller.cc
 *    Description:  Customized chiller plant
 *                  (free cooling, tower, chiller model)  
 *
 * ===================================================================
 */

#include "custom-chiller.h"

#include <math.h>

constexpr int CustomChiller::rangeLUT[NUM_RANGE_VALUES];
constexpr double CustomChiller::wetBulbLUT[NUM_WETBULB_SETTINGS];
constexpr double CustomChiller::approachLUT[NUM_RANGE_VALUES][NUM_WETBULB_SETTINGS];

CustomChiller::CustomChiller( const ChillerParams &params )
    : Chiller(params)
{
    LOG_INFO << "CustomChiller created";
    m_deltaTempAirTower = (MAXIMUM_HEAT_TOWER/KW_TON_CONVERSION)
        /(RHO_AIR*CP_AIR*1000*MAXIMUM_TOWER_AIRFLOW/CFM_CONVERSION);
    VLOG_2 << "Tower air temperature variation is: " << m_deltaTempAirTower;
}

CustomChiller::~CustomChiller()
{
}

//Power models
double CustomChiller::computePower(CoolingParams *params)
{
    // Reset variables
    m_chillerPower = 0; m_towerPower=0;
    m_electricPower = 0;
    // Compute heat
    double heat = params->waterFlow*GPM_CONVERSION*RHO_WATER*CP_WATER*1000
        *(params->waterTemp-m_params.coldWaterTemp);
    // See if we can use free cooling
    if (m_params.coldWaterTemp == FREE_COOLING_WATER_TEMP){
        // Try free cooling
        bool free = computeFreeCooling (heat, params);
        if (!free){
            params->freeCooling = false;
            params->converged = false;
            return 0.0;
        }
        m_electricPower = m_towerPower;
        params->freeCooling = true;
        params->converged = true;
        return m_electricPower;
    }
    // Use chiller
    params->freeCooling = false;
    params->converged = true;
    computeChillerPower(heat, params);
    m_electricPower = m_chillerPower + m_towerPower;
    return m_electricPower;
}

void CustomChiller::computeChillerPower(double heat, CoolingParams *params)
{
    //Assume that tower extracts as much heat from water as possible
    double approach = getTowerApproach(m_params.outdoorTemp+COOLING_TOWER_DESIGN_RANGE,
                                       m_params.outdoorTemp, m_params.outdoorTemp);

    //Compute Tct,cold == Tcondenser
    double tempCTcold = m_params.outdoorTemp + approach;
    VLOG_1 << "Temperature in heat exchanger is: " << tempCTcold
           << "( outdoor=" << m_params.outdoorTemp << " approach=" << approach << ")";

    // Checking for pre-cooling
    double tempCondenser = tempCTcold;
    double tempEvap = params->waterTemp;
    double preCooling=0;
    if ( params->waterTemp > ( tempCTcold + HEAT_EXCHANGER_APPROACH) ){
        double deltaT = (params->waterTemp - tempCTcold) - HEAT_EXCHANGER_APPROACH;
        tempEvap = params->waterTemp - deltaT;
        tempCondenser = tempCTcold + deltaT;
        preCooling = RHO_WATER*CP_WATER*1000*params->waterFlow*GPM_CONVERSION*deltaT;
        VLOG_1 << "Using pre-cooling mode to decrease Tcw,hot temperature. "
               << "Deacrease of " << deltaT << "C and heat extracted: " << preCooling;
    }
    
    double cop = getChillerCOP(tempEvap, tempCondenser);
    m_chillerPower = (heat - preCooling)/cop;
    if (m_chillerPower < 0){
        m_chillerPower = 0;
    }
    VLOG_1 << "Chiller needs to extract " << m_chillerPower << " (COP= " << cop << ")";
    double towerHeat = heat + m_chillerPower;
    VLOG_2 << "Amount of heat in tower is: " << towerHeat;
    
    // Calculate hot water return to tower
    double tempTowerHot = tempCTcold
        + towerHeat/(RHO_WATER*CP_WATER*1000*params->waterFlow*GPM_CONVERSION);
    VLOG_1 << "Tower: hot water return temperature is " << tempTowerHot
           << "(and cold supply is: " << tempCTcold << ")";

    // Calculate tower power
    double airflowTower = towerHeat/(RHO_AIR*CP_AIR*1000*m_deltaTempAirTower)*CFM_CONVERSION;
    m_towerPower = MAXIMUM_TOWER_FAN_POWER*pow((airflowTower/MAXIMUM_TOWER_AIRFLOW), 3);
    VLOG_2 << "Airflow for tower is: " << airflowTower << " and power: " << m_towerPower;

}

bool CustomChiller::computeFreeCooling(double heat, CoolingParams *params)
{
    //Assert for approach: (number of degrees I can extract from Tct,hot)
    double approach = getTowerApproach(params->waterTemp, m_params.coldWaterTemp,
                                       m_params.outdoorTemp);
    if ( (approach+m_params.outdoorTemp) > (m_params.coldWaterTemp - HEAT_EXCHANGER_APPROACH) ) {
            VLOG_1 << "Approach assert violated: coldWaterTemp= " << m_params.coldWaterTemp
                   << " outdoorTemp= " << m_params.outdoorTemp
                   << " approach= " << approach ;
        return false;
    }

    // Calculate hot water return to tower
    double tempTowerHot = params->waterTemp;
    VLOG_2 << "Free cooling: hot water temperature is " << tempTowerHot;
    
    // Can use free cooling.
    double airflowTower = heat/(RHO_AIR*CP_AIR*1000*m_deltaTempAirTower)*CFM_CONVERSION;
    m_towerPower = MAXIMUM_TOWER_FAN_POWER*pow((airflowTower/MAXIMUM_TOWER_AIRFLOW), 3);
    VLOG_2 << "Free cooling can be used! Airflow for tower is: " << airflowTower
           << " and power: " << m_towerPower;
    return true;
}

double CustomChiller::getTowerApproach(double hotWaterTemp, double coldWaterTemp,
                                       double wetBulbTemp)
{
    VLOG_1 << "hotWaterTemp= " << hotWaterTemp << " coldWaterTemp= " << coldWaterTemp
           << "wetBulbTemp= " << wetBulbTemp;
    int load; int loadIdx =0;
    if ( (hotWaterTemp - coldWaterTemp) > COOLING_TOWER_DESIGN_RANGE ){
        VLOG_1 << "Trying to extract too much heat from water (above design!)";
        load = 100;
        loadIdx = 0;
    }
    else if ( (hotWaterTemp - coldWaterTemp) == COOLING_TOWER_DESIGN_RANGE ){
        LOG_INFO << "Tower working at maximum capacity to extract as much heat"
                 << "from water as possible";
        load = 100;
        loadIdx = 0;
    }
    else {
        load = ((hotWaterTemp-coldWaterTemp)*100)/COOLING_TOWER_DESIGN_RANGE;
        // Getting load (number of degrees water heated)
        loadIdx = NUM_RANGE_VALUES - 1;
        for (int i=1; i< NUM_RANGE_VALUES; i++){            
            if (load > rangeLUT[i]){
                // Computing distance to load                
                loadIdx = i;
                break;
            }
        }
    }
    VLOG_1 << "Load is: " << load;
    
    // Computing wetbulb
    int wetBulbIdx = computeWetBulbIdx(wetBulbTemp);
        
    // Getting tower approach
    double approach = approachLUT[loadIdx][wetBulbIdx];
    VLOG_1 << "Approach is: " << approach << " (loadIdx= " << loadIdx
           << " wetBulbIdx = " << wetBulbIdx << ")";
    return approach;
}

bool CustomChiller::reduceWaterTemp()
{
    if (m_params.coldWaterTemp == FREE_COOLING_WATER_TEMP){
        m_params.coldWaterTemp = CHILLER_WATER_TEMP;
        return true;
    }
    return false;
}

double CustomChiller::getChillerCOP(double tempEvap, double tempCondenser)
{
    double cop;
    if (tempCondenser < tempEvap){
        //This is pre-cooling
        cop = MAX_COP_VALUE;
        VLOG_1 << "Condenser temperature lower than evap. temperature."
               << "Chiller COP set to maximum: " << MAX_COP_VALUE;
        return cop;
    }
    cop = tempEvap/(tempCondenser-tempEvap);
    if (cop > MAX_COP_VALUE){
        cop=MAX_COP_VALUE;
    }
    else if(cop < MIN_COP_VALUE){
        cop=MIN_COP_VALUE;
    } 
    VLOG_2 << "Chiller COP is: " << cop;
    return cop;
}

double CustomChiller::getTevapForMaxCOP(double tempOut, double load)
{
    int wetBulbIdx = computeWetBulbIdx(tempOut);
    int loadIdx = NUM_RANGE_VALUES - 1;
    for (int i=1; i< NUM_RANGE_VALUES; i++){            
        if (load > rangeLUT[i]){
            // Computing distance to load                
            loadIdx = i;
            break;
        }
    }
    if (load == 100){
        loadIdx = 0;
    }
    double approach = approachLUT[loadIdx][wetBulbIdx];
    double tempCondenser = tempOut + approach;
    double tempEvap = tempCondenser*MAX_COP_VALUE/(1+MAX_COP_VALUE) + CustomChiller::HEAT_EXCHANGER_APPROACH;

    VLOG_1 << "Minimum evaporator temperature for max COP is: " << tempEvap;
    return tempEvap;
}

//@brief: compute the maximum heat that can be extracted... as a result I give a temperature
// i.e. the approach
double CustomChiller::assertApproach(double tempOut)
{
    if ( CustomChiller::FREE_COOLING_WATER_TEMP > ( tempOut + CustomChiller::HEAT_EXCHANGER_APPROACH )){
        int wetBulbIdx = computeWetBulbIdx(tempOut);
        for (unsigned int i=0; i<NUM_RANGE_VALUES; i++){
            double approach = approachLUT[i][wetBulbIdx];
            if (CustomChiller::FREE_COOLING_WATER_TEMP >
                (tempOut + CustomChiller::HEAT_EXCHANGER_APPROACH + approach)){
                double load = rangeLUT[i];
                VLOG_1 << "Maximum load= " << load << " for approach= " << approach
                       << " and wetbulb= " << tempOut
                       << " loadIdx= " << i << " wetBulbIdx= " << wetBulbIdx;
                return load;
            }
        }
    }
    // Free cooling not possible
    return 0.0;
}

int CustomChiller::computeWetBulbIdx(double wetBulbTemp)
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

double CustomChiller::getMaxApproach(double tempOut)
{
    return getTowerApproach(tempOut+COOLING_TOWER_DESIGN_RANGE,
                            tempOut, tempOut);
}

double CustomChiller::getFreeCoolingWaterTemp()
{
    return CustomChiller::FREE_COOLING_WATER_TEMP;
}

double CustomChiller::getChillerWaterTemp()
{
    return CustomChiller::CHILLER_WATER_TEMP;
}

double CustomChiller::getTowerRange()
{
    return CustomChiller::COOLING_TOWER_DESIGN_RANGE;
}
