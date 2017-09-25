/*
 * DevsSlide.h
 *
 *  Created on: 13/07/2016
 *      Author: jlrisco
 */

#ifndef SRC_XDEVS_DEVSSLIDE_H_
#define SRC_XDEVS_DEVSSLIDE_H_

// C++
#include <map>
#include <list>

// XML
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLInteger.hpp>


// DEVS
#include "../../lib/xdevs/xdevs/core/modeling/Coupled.h"

#include "DevsJobsGenerator.h"
#include "DevsRoom.h"
#include "DevsWeather.h"
#include "DevsEnergyCalculator.h"
#include "DevsCooling.h"
#include <fstream>
#include <iostream>

using namespace std;

class DevsSlide : public Coupled {
public:
	DevsSlide(std::string OutFile, std::string JobFile, std::string WeatherFile, std::string configFile,double initialTime);
	virtual ~DevsSlide();

	/*
	 * This function recieves a XML File with the initial configuration of the Room, parses it and generates 3 data structures,
	 * ircParams: has the inlet temperatures for each IRC
	 * serverFanSpeeds: the name is not correct, it also has the number of cores, the dynamic power of the cpu and the memory added to the server
	 * return value: contains the basic structure of the system
	 * 	-dataCenterStructure[IRC][Rack][Server]
	 */
	std::map<std::string, std::map<std::string, std::list<std::string>>> parseXmlStructure(const std::string& xmlFile, std::map<std::string, int>& ircParams, std::map<std::string, std::map<std::string, double[4]>>& serverFanSpeeds);

	/*
	 * For a DOMNode returns the childNode with name nombreHijo
	 */
	static xercesc::DOMNode* xmlDameNodoHijo(xercesc::DOMNode* padre, std::string nombreHijo);
    static xercesc::DOMNodeList* xmlDameNodosHijos(xercesc::DOMNode* padre, std::string nombreHijo);
    bool existe(std::string fichero);
};

#endif /* SRC_XDEVS_DEVSSLIDE_H_ */
