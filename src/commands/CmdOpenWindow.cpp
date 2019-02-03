#include "CmdOpenWindow.h"

CmdOpenWindow::CmdOpenWindow(WindowBase& p_Window)
  : m_Window(p_Window)
{

}

CmdOpenWindow::~CmdOpenWindow()
{
}

void CmdOpenWindow::Execute() const
{
  if(m_Window.IsOpen())
  {
    return;
  }

  // By giving this as parameter the menu associated to the to be
  // opened window will be browsed for a menu item which user data
  // contains a pointer to this command. If found this menu item will
  // then be disabled at window opening time and enabled when the
  // window is closed.
  m_Window.Open((APTR)this);
}
