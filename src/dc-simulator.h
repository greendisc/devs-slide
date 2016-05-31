/*
 * =====================================================================================
 *
 *       Filename:  dc-simulator.hh
 *
 *    Description:  Overall-DC power consumption simulator 
 *
 * =====================================================================================
 */

#ifndef DC_SIMULATOR
#define DC_SIMULATOR

#include "cooling/chiller.h"
#include "cooling/pump.h"
#include "room/rack-irc.h"
#include "room/room.h"

class DCSimulator
{
    
public:

    // Structs 
    typedef struct {
        std::string dcName;
        Chiller::ChillerParams chiller;
        std::string pumpType;
        std::string roomType;
        std::string ircType;
        std::vector<RackAndIRC::RackAndIRCParams> rackIRC;
    } DCParams;
    
    //Constructor and destructor
    DCSimulator( DCParams &layout );
    ~DCSimulator();

    //Constant definitions
    static constexpr int EVERYSECS = 2592000;
    //FIXME-marina: this is SUPER error prone!!
    static constexpr int NUMSTATISTICS = 8;
    
    //Getters and setters
    Room* getRoom();
    Chiller* getChiller();

    //Compute power consumption (assume all needed data is loaded)
    void run();
    void computePower(double outdoorTemp);
    void computePower();
    int applyFSPolicy(std::string policy);
    
    // Print results
    void printAllPowerValues( time_t time, double tempOut, std::ofstream &powerOutFile );
    void printEnergyStatistics (std::string &file);
        
private:
    //Attributes
    Chiller* m_chiller;
    Pump* m_pump;
    Room* m_room;

};

#endif //DC_SIMULATOR
