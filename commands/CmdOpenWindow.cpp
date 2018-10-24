#include <clib/dos_protos.h>
#include "AslFileRequest.h"
#include "DosFile.h"
#include "TextDocument.h"
#include "CmdOpenWindow.h"


CmdOpenWindow::CmdOpenWindow(WindowBase& p_Window, AppMenu& p_Menu)
  : m_Window(p_Window),
    m_Menu(p_Menu)
{

}

CmdOpenWindow::~CmdOpenWindow()
{
  m_Window.Close();
}

void CmdOpenWindow::Execute()
{
  if(m_Window.Open() == true)
  {
    if(m_Menu.IntuiMenu() != NULL)
    {
      m_Menu.AttachToWindow(&m_Window);
    }
  }
/*
  // Let the user select a file to open
  AslFileRequest request(m_pWindow->IntuiWindow());
  SimpleString fileName = request.SelectFileName("Select file to open");
  if(fileName.Length() == 0)
  {
    return;
  }

  // Load the file into the Document's TextDocument
  if(m_TextDocument.Load(fileName) == false)
  {
    return;
  }

  // Set the File into the Document's window
  m_pWindow->SetContent(&m_TextDocument);
*/
}
