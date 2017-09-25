/*
 * DevsRoom.h
 *
 *  Created on: 27/04/2017
 *      Author: roberto
 */

#ifndef SRC_XDEVS_DEVSROOM_H_
#define SRC_XDEVS_DEVSROOM_H_

// C++
#include <map>
#include <list>

//Devs
#include "../../lib/xdevs/xdevs/core/modeling/Coupled.h"

//Atomicos
#include "DevsServer.h"
#include "DevsRack.h"
#include "DevsIrc.h"
#include "DevsAllocator.h"

class DevsRoom: public Coupled {
public:
	DevsRoom(
			std::map<std::string, std::map<std::string, std::list<std::string>>>& dataCenterStructure, std::map<std::string, int>& ircInlets,
			std::map<std::string, std::map<std::string, double[4]>>& serverFanSpeeds, std::list<std::string>& ircNames);
			virtual ~DevsRoom();

			//AllocatorPorts
			Port iJob;
			//IRCPorts
			Port iWaterTemp;
			std::map<std::string, Port*> oIRCGallons;
			std::map<std::string, Port*> oIRCPower;
			std::map<std::string, Port*> oIRCITPower;
			std::map<std::string, Port*> oIRCFSPower;
			std::map<std::string, Port*> oIRCHotWater;
			std::map<std::string, Port*> oIRCAvgTempCpu;
			std::map<std::string, Port*> oIRCMaxTempCpu;

		};

#endif /* SRC_XDEVS_DEVSROOM_H_ */
