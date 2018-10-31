#include <clib/dos_protos.h>
#include "AslFileRequest.h"
#include "DosFile.h"
#include "TextDocument.h"
#include "CmdOpenWindow.h"


CmdOpenWindow::CmdOpenWindow(WindowBase& p_Window)
  : m_Window(p_Window)
{

}

CmdOpenWindow::~CmdOpenWindow()
{
}

void CmdOpenWindow::Execute()
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
  m_Window.Open(this);
}
