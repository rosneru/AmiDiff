#include "CmdQuit.h"


CmdQuit::CmdQuit(SimpleString p_Name, bool& p_bExitRequested)
  : Command(p_Name),
    m_bExitRequested(p_bExitRequested)
{
}

CmdQuit::~CmdQuit()
{

}

void CmdQuit::Execute()
{
  m_bExitRequested = true;
}
