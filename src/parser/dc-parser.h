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

#include "cpprest/basic_types.h"
#include <sys/time.h>
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include <tclap/CmdLine.h>

#include "../dc-simulator.h"
#include "../generic_defs.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;
using namespace web::json;

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

    static void parseDCConfig(json::value &config, DCSimulator::DCParams &layout);
    static bool parseLayout(json::value &layout, DCSimulator::DCParams &params);

    static bool parseChiller(json::value &config, Chiller::ChillerParams &chiller);
    static bool parseRacksAndIRCs(json::array &itArray,
                                  std::vector<RackAndIRC::RackAndIRCParams> &rackIRC );
    static bool parseRackAndServers(json::value &rackInfo, Rack::RackParams &rack);

    static bool addRoomParams(json::value &config, DCSimulator &sim);
    static bool addWorkload(json::value &config, DCSimulator &sim);
    
    virtual ~DCParser();

};



#endif /* DC_PARSER_H */

