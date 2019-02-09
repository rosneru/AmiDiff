#include "CmdQuit.h"


CmdQuit::CmdQuit(bool& p_bExitAllowed, bool& p_bExitRequested)
  : m_bExitAllowed(p_bExitAllowed),
    m_bExitRequested(p_bExitRequested)
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
