/*
 * ===================================================================
 *
 *       Filename:  dc-parser.hh
 *    Description:  Data center Layout, Params and workload parser 
 *
 * ===================================================================
 */

#ifndef DC_PARSER_H
#define DC_PARSER_H

#include "../dc-simulator.h"
#include "../generic_defs.h"

// XML
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

class DCParser
{
public:

    typedef struct {
        std::string configFile;
        std::string weatherFile;
        std::string powerOutFile;
        int port;
        time_t baseTime;
        time_t firstSubmit;
        bool offline;
        std::string jobLoggerFile;
        std::string allocPath;
        std::string coolPolicy;
    }CmdLineArgs;

    static constexpr int FIRST_SUBMISSION_TIME = 600;
    
    static bool parseCmdLineArgs(int argc, char *argv[], CmdLineArgs &parsedArgs);

    static void parseDCConfig(DCSimulator::DCParams &layout);
    static bool parseLayout(xercesc::DOMNode* xmlDataCenter, DCSimulator::DCParams &params);

    static bool parseChiller(xercesc::DOMNode* xmlChiller, Chiller::ChillerParams &chiller);
    static bool parseRacksAndIRCs(xercesc::DOMNodeList* xmlITs, std::vector<RackAndIRC::RackAndIRCParams> &rackIRC );
    static bool parseRackAndServers(xercesc::DOMNode* rackInfo, Rack::RackParams &rack);

    static bool addRoomParams(xercesc::DOMNode* dataCenter, DCSimulator &sim);
    static bool addWorkload(xercesc::DOMNode* dataCenter, DCSimulator &sim);
    
    static xercesc::DOMNode* xmlDameNodoHijo(xercesc::DOMNode* padre, std::string nombreHijo);
    static std::string xmlDameAtributo(xercesc::DOMNode* nodo, std::string atributo);


    virtual ~DCParser();

};



#endif /* DC_PARSER_H */

