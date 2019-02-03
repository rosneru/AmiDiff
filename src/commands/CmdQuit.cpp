#include "CmdQuit.h"


CmdQuit::CmdQuit(bool& p_bExitRequested)
  : m_bExitRequested(p_bExitRequested)
{
}

CmdQuit::~CmdQuit()
{

}

void CmdQuit::Execute() const
{
  m_bExitRequested = true;
}
