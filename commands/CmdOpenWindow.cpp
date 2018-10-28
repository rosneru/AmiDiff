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

  m_Window.Open();
}
