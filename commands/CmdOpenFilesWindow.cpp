#include <clib/dos_protos.h>
#include "AslFileRequest.h"
#include "DosFile.h"
#include "TextDocument.h"
#include "CmdOpenFilesWindow.h"


CmdOpenFilesWindow::CmdOpenFilesWindow(OpenFilesWindow& p_OpenFilesWindow)
  : m_OpenFilesWindow(p_OpenFilesWindow)
{

}

CmdOpenFilesWindow::~CmdOpenFilesWindow()
{
  m_OpenFilesWindow.Close();
}

void CmdOpenFilesWindow::Execute()
{
  m_OpenFilesWindow.Open();
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
