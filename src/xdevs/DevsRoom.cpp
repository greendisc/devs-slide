/*
 * DevsRoom.cpp
 *
 *  Created on: 27/04/2017
 *      Author: roberto
 */

#include "DevsRoom.h"

DevsRoom::DevsRoom(
		std::map<std::string, std::map<std::string, std::list<std::string>>>& dataCenterStructure,
std::map<std::string, int>& ircInlets,
std::map<std::string, std::map<std::string, double[4]>>& serverFanSpeeds,
std::list<std::string>& ircNames):
Coupled("Room"), iJob("iJob"), iWaterTemp("iWaterTemp") {

	DevsAllocator* allocator = new DevsAllocator("Allocator", ircNames);
	Coupled::addComponent(allocator);
	this->addInPort(&iJob);
	Coupled::addCoupling(this, &(this->iJob), allocator, &(allocator->iJob));

	for (std::list<std::string>::iterator it=ircNames.begin(); it != ircNames.end(); ++it) {
		Port* port = new Port(*it);
		oIRCGallons[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCHotWater[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCPower[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCITPower[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCFSPower[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCAvgTempCpu[*it] = port;
		this->addOutPort(port);

		port = new Port(*it);
		oIRCMaxTempCpu[*it] = port;
		this->addOutPort(port);
	}

	this->addInPort(&iWaterTemp);

	for(std::list<std::string>::iterator itrIrc = ircNames.begin(); itrIrc!=ircNames.end(); ++itrIrc) {
		std::string ircName = *itrIrc;
		std::map<std::string, std::list<std::string>> racksPerIrc = dataCenterStructure[ircName];
		std::list<std::string> rackNames;
		for(std::map<std::string, std::list<std::string>>::iterator itr = racksPerIrc.begin(); itr!=racksPerIrc.end(); ++itr) {
			rackNames.push_back(itr->first);
		}
		DevsIrc* irc = new DevsIrc(ircName, rackNames);
		Coupled::addComponent(irc);
		Coupled::addCoupling(allocator, allocator->oIrcs[ircName], irc, &(irc->iJob));
		Coupled::addCoupling(irc, &(irc->oIRCGallons), this, this->oIRCGallons[ircName]);
		Coupled::addCoupling(irc, &(irc->oIRCHotWater), this, this->oIRCHotWater[ircName]);
		Coupled::addCoupling(this, &(this->iWaterTemp), irc, &(irc->iWaterTemp));
		Coupled::addCoupling(irc, &(irc->oIRCPower),this, this->oIRCPower[ircName]);
		Coupled::addCoupling(irc, &(irc->oIRCITPower),this, this->oIRCITPower[ircName]);
		Coupled::addCoupling(irc, &(irc->oIRCFSPower),this, this->oIRCFSPower[ircName]);
		Coupled::addCoupling(irc, &(irc->oIRCAvgTempCpu),this, this->oIRCAvgTempCpu[ircName]);
		Coupled::addCoupling(irc, &(irc->oIRCMaxTempCpu),this, this->oIRCMaxTempCpu[ircName]);

		// 4.- Racks
		for(std::list<std::string>::iterator itrRack = rackNames.begin(); itrRack!=rackNames.end(); ++itrRack) {
			std::string rackName = *itrRack;
			std::list<std::string> serverNames = racksPerIrc[rackName];
			DevsRack* rack = new DevsRack(rackName, serverNames);
			Coupled::addComponent(rack);
			Coupled::addCoupling(irc, irc->oJob[rackName], rack, &(rack->iJob));
			Coupled::addCoupling(rack, &(rack->oRackPowerIT), irc, irc->iRacksPowerIT[rackName]);
			Coupled::addCoupling(rack, &(rack->oRackPowerFS), irc, irc->iRacksPowerFS[rackName]);
			Coupled::addCoupling(rack, &(rack->oRackAirflow), irc, irc->iRacksAirflow[rackName]);
			Coupled::addCoupling(rack, &(rack->oRackTempOut), irc, irc->iRacksTempOut[rackName]);
			Coupled::addCoupling(rack, &(rack->oRackAvgTempCpu), irc, irc->iRacksAvgTempCpu[rackName]);
			Coupled::addCoupling(rack, &(rack->oRackMaxTempCpu), irc, irc->iRacksMaxTempCpu[rackName]);

			// 5.- Servers
			for(std::list<std::string>::iterator itrServer = serverNames.begin(); itrServer!=serverNames.end(); ++itrServer) {
				std::string serverName = *itrServer;
				DevsServer* server = new DevsServer(serverName);
				server->setInlet(ircInlets[*itrIrc]);
				server->setInitialValues(serverFanSpeeds[*itrRack][serverName]);
				Coupled::addComponent(server);
				Coupled::addCoupling(rack, rack->oJob[serverName], server, &(server->iJob));
				Coupled::addCoupling(server, &(server->oServerPowerIT), rack, rack->ports[serverName].iServersPowerIT);
				Coupled::addCoupling(server, &(server->oServerPowerFS), rack, rack->ports[serverName].iServersPowerFS);
				Coupled::addCoupling(server, &(server->oServerAirflow), rack, rack->ports[serverName].iServersAirflow);
				Coupled::addCoupling(server, &(server->oServerTempOut), rack, rack->ports[serverName].iServersTempOut);
				Coupled::addCoupling(server, &(server->oServerAvgTempCpu), rack, rack->ports[serverName].iServersAvgTempCpu);
				Coupled::addCoupling(server, &(server->oServerMaxTempCpu), rack, rack->ports[serverName].iServersMaxTempCpu);
			}
		}
	}

}

DevsRoom::~DevsRoom() {
	std::list<Component*> components = this->getComponents();
	for(std::list<Component*>::iterator itr = components.begin(); itr!=components.end(); ++itr) {
		delete *itr;
	}
}

