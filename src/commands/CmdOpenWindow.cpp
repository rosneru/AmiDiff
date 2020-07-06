#include "CmdOpenWindow.h"

CmdOpenWindow::CmdOpenWindow(Array<WindowBase*>& windowArray,
                             WindowBase& window)
  : CommandBase(windowArray),
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
