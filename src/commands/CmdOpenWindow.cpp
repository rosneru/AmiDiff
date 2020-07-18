#include "CmdOpenWindow.h"

CmdOpenWindow::CmdOpenWindow(std::vector<WindowBase*>* pAllWindowsVector,
                             WindowBase& window)
  : CommandBase(pAllWindowsVector),
    m_Window(window)
{

}

CmdOpenWindow::~CmdOpenWindow()
{
}

void CmdOpenWindow::Execute(struct Window* pActiveWindow) 
{
  if(m_Window.IsOpen())
  {
    return;
  }

  
  if(m_Window.Open() == true)
  {
    DisableInAllWindowMenus();
  }
}
