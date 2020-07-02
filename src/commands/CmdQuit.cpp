#include "CmdQuit.h"


CmdQuit::CmdQuit(Array<WindowBase*>& windowArray,
                 bool& exitAllowed, 
                 bool& exitRequested)
  : CommandBase(windowArray),
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
