#include "CmdCloseWindow.h"

CmdCloseWindow::CmdCloseWindow(std::vector<WindowBase*>* pAllWindowsVector,
                               const CmdOpenWindow& openWindowCommand,
                               WindowBase& window)
  : CommandBase(pAllWindowsVector),
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

  // Re-enable the command that was used to open the window in all menus
  m_OpenWindowCommand.EnableInAllWindowMenus();
}
