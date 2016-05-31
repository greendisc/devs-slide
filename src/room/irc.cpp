/*
 * ===================================================================
 *
 *       Filename:  irc.cc
 *    Description:  In-row cooler base class  
 *
 * ===================================================================
 */

#include "irc.h"

IRC::IRC()
{    
    m_inletTemp=DEFAULT_INLET_TEMPERATURE;    
}

IRC::~IRC()
{
}

void IRC::setInletTemp(int temp)
{
    m_inletTemp=temp;
}

int IRC::getInletTemp()
{
    return m_inletTemp;
}

