#include "CmdQuit.h"


CmdQuit::CmdQuit(bool& exitAllowed, bool& exitRequested)
  : m_bExitAllowed(exitAllowed),
    m_bExitRequested(exitRequested)
{
}

CmdQuit::~CmdQuit()
{

}

void CmdQuit::Execute() const
{
  if(m_bExitAllowed)
  {
    m_bExitRequested = true;
  }
}
