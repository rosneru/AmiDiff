#include "CmdQuit.h"


CmdQuit::CmdQuit(std::vector<WindowBase*>* pAllWindowsArray,
                 bool& exitAllowed, 
                 bool& exitRequested)
  : CommandBase(pAllWindowsArray),
    m_bExitAllowed(exitAllowed),
    m_bExitRequested(exitRequested)
{
}

CmdQuit::~CmdQuit()
{

}

void CmdQuit::Execute(struct Window* pActiveWindow)
{
  if(m_bExitAllowed)
  {
    m_bExitRequested = true;
  }
}
