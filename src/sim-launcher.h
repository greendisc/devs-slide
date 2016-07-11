#ifndef _SIM_LAUNCHER_H_
#define _SIM_LAUNCHER_H_

#include <string>
#include <vector>
#include <list>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <sys/time.h>

#include "alloc/allocator.h"
#include "dc-factory.h"
#include "dc-simulator.h"
#include "generic_defs.h"
#include "parser/dc-parser.h"
#include "weather/weather.h"

class SimLauncher {
public:
	SimLauncher();
	~SimLauncher();
	void loadConfig(DCParser::CmdLineArgs &cmdArgs);
	int getJobLoggerLine(DCSimulator &sim, Allocator &alloc);
	/*json::value config;
	int udpsock;
	bool noLog;*/
	xercesc::DOMNode* xmlDataCenter;
	std::ifstream weatherfile;
	std::ofstream powerfile;
	std::ifstream jobfile;
};


#endif
