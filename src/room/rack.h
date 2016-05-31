/*
 * ===================================================================
 *
 *       Filename:  rack.hh
 *    Description:  Rack base class 
 *
 * ===================================================================
 */

#include <vector>
#include <list>
#include <map>

#include "server.h"

class Rack
{
public:

    typedef struct{
        std::string rackName;
        std::map<std::string, std::string> servers;
    }RackParams;
    
    Rack();
    virtual ~Rack();

    std::map<std::string, Server*> m_servers;
    
protected:


private:

};


