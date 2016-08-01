/*
 * main_temp.cpp
 *
 *  Created on: 15 de jul. de 2016
 *      Author: jlrisco
 */

#include <fstream>
#include <iostream>
#include "../parser/dc-parser.h"
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

bool getNextJob(std::ifstream& jobsFile, std::string& jobType, std::string& jobTime, std::string& jobId) {
	if(!jobsFile.eof()) {
		std::getline(jobsFile, jobType, ';');
		std::getline(jobsFile, jobTime, ';');
		std::getline(jobsFile, jobId);
		if(jobType.size()>0) {
			return true;
		}
	}
	return false;
}

bool getNextAllocParam(std::ifstream& allocFile, std::string& ircName,std::string&  serverName,std::string&  numThreads) {
	if(!allocFile.eof()) {
		std::getline(allocFile, ircName, ',');
		std::getline(allocFile, serverName, ',');
		std::getline(allocFile, numThreads);
		if(ircName.size()>0 && serverName.size()>0 && numThreads.size()>0) {
			ircName = ircName.substr(2,ircName.size()-3);
			serverName = serverName.substr(1,serverName.size()-2);
			numThreads = numThreads.substr(0,numThreads.size()-1);
			return true;
		}
	}
	return false;
}

bool getNextJobParam(std::ifstream& paramFilePath, std::string& numCores, std::string& cpuPower, std::string& memPower) {
	if(!paramFilePath.eof()) {
		std::getline(paramFilePath, numCores, ',');
		std::getline(paramFilePath, cpuPower, ',');
		std::getline(paramFilePath, memPower);
		if(numCores.size()>0 && cpuPower.size()>0 && memPower.size()>0) {
			numCores = numCores.substr(1,numCores.size()-1);
			memPower = memPower.substr(0,memPower.size()-1);
			return true;
		}
	}
	return false;
}

bool getRackName(xercesc::DOMDocument* xmlConfigFile, const std::string& ircName, const std::string& serverName, std::string& rackName) {
	xercesc::DOMNodeList* xmlRackIRCCouples = xmlConfigFile->getElementsByTagName(xercesc::XMLString::transcode("RackIRCCouple"));
	for(unsigned int i=0; i<xmlRackIRCCouples->getLength(); ++i) {
		xercesc::DOMNode* xmlRackIRCCouple = xmlRackIRCCouples->item(i);
		xercesc::DOMNode* xmlIrcName = DCParser::xmlDameNodoHijo(xmlRackIRCCouple, "name");
		std::string tempIrcName = xercesc::XMLString::transcode(xmlIrcName->getFirstChild()->getNodeValue());
		if(tempIrcName==ircName) {
			// We must find the server
			xercesc::DOMNode* xmlEquipment = DCParser::xmlDameNodoHijo(xmlRackIRCCouple, "equipment");
			xercesc::DOMNode* xmlRack = DCParser::xmlDameNodoHijo(xmlEquipment, "Rack");
			xercesc::DOMNode* xmlRackName = DCParser::xmlDameNodoHijo(xmlRack, "name");
			rackName = xercesc::XMLString::transcode(xmlRackName->getFirstChild()->getNodeValue());
			xercesc::DOMNodeList* xmlServers = xmlRack->getChildNodes();
			for(unsigned int j=0; j<xmlServers->getLength(); ++j) {
				xercesc::DOMNode* xmlNode = xmlServers->item(j);
				std::string nodeName = xercesc::XMLString::transcode(xmlNode->getNodeName());
				if(nodeName!="Servers") {
					continue;
				}
				std::string tempServerName = xercesc::XMLString::transcode(xmlNode->getFirstChild()->getNodeValue());
				if(tempServerName==serverName) {
					return true;
				}
			}
		}
	}
	return false;
}

int main() {
	std::string layerFilePath = "../../../../Trabajo/Pendiente/slurm-allocation-data/slurm/ConfigFile.xml";
	std::string jobsFilePath = "../../../../Trabajo/Pendiente/slurm-allocation-data/slurm/joblogger-default-slurm.txt";
	std::string newJobsFilePath = "../../../../Trabajo/Pendiente/slurm-allocation-data/JobLogger.txt";
	xercesc::XMLPlatformUtils::Initialize();
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
    parser->parse(layerFilePath.c_str());
    xercesc::DOMDocument* xmlConfigFile = parser->getDocument();
	std::string jobType, jobTime, jobId;
	std::ifstream jobsFile;
	jobsFile.open(jobsFilePath);
	bool thereIsJob = getNextJob(jobsFile, jobType, jobTime, jobId);
	// We need several fields:
	// IRC name, from alloc_out_<id>.txt
	// Server name, from alloc_out_<id>.txt
	// Rack name, taken from layout
	// Num threads, from alloc_out_<id>.txt
	// Num cores, from param_job_<id>.txt
	// CPU power, from param_job_<id>.txt
	// Mem power, from param_job_<id>.txt
	std::string ircName, serverName, rackName, numThreads, numCores, cpuPower, memPower;
	std::ifstream allocFile;
	std::ifstream paramFile;

	std::ofstream newJobsFile;
	newJobsFile.open(newJobsFilePath);
	while(thereIsJob) {
		if(jobType=="exit") {
			jobTime = ""; jobId = "";
			ircName = ""; serverName = ""; rackName = ""; numThreads = ""; numCores = ""; cpuPower = ""; memPower = "";
		}
		else {
			std::string allocFilePath = "../../../../Trabajo/Pendiente/slurm-allocation-data/slurm/files-default-slurm/alloc_out_" + jobId + ".txt";
			std::string paramFilePath = "../../../../Trabajo/Pendiente/slurm-allocation-data/slurm/files-default-slurm/param_job_" + jobId + ".txt";
			allocFile.open(allocFilePath);
			paramFile.open(paramFilePath);
			getNextAllocParam(allocFile, ircName, serverName, numThreads);
			getNextJobParam(paramFile, numCores, cpuPower, memPower);
			allocFile.close();
			paramFile.close();
			// Now we must find the rack name
			getRackName(xmlConfigFile, ircName, serverName, rackName);
		}
		newJobsFile << jobType << ";" << jobTime << ";" << jobId << ";" << ircName << ";" << rackName << ";" << serverName << ";" << numThreads << ";" << numCores << ";" << cpuPower << ";" << memPower << std::endl;
		thereIsJob = getNextJob(jobsFile, jobType, jobTime, jobId);
	}
	newJobsFile.close();
	jobsFile.close();
	xercesc::XMLPlatformUtils::Terminate();
	return 0;
}

