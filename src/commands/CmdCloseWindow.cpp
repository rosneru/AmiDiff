#include "CmdCloseWindow.h"

CmdCloseWindow::CmdCloseWindow(Array<WindowBase*>& windowArray,
                               const CmdOpenWindow& openWindowCommand,
                               WindowBase& window)
  : CommandBase(windowArray),
    m_OpenWindowCommand(openWindowCommand),
    m_Window(window)
{

}

CmdCloseWindow::~CmdCloseWindow()
{
}

void CmdCloseWindow::Execute(struct Window* pActiveWindow) 
{
  if(!m_Window.IsOpen())
  {
    return;
  }

  m_Window.Close();

  // Re-enable the 
  m_OpenWindowCommand.EnableInAllWindowMenus();
}
