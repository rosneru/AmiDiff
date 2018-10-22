#include <clib/dos_protos.h>
#include "AslFilerequest.h"
#include "TextDocument.h"
#include "CmdFileOpen.h"


CmdFileOpen::CmdFileOpen(SimpleString p_Name, TextDocument& p_TextDocument, TextWindow* p_pWindow)
  : m_OpenFilesWindow(p_pWindow->OnScreen()),
    m_TextDocument(p_TextDocument),
    m_pWindow(p_pWindow),
    Command(p_Name)
{

}

CmdFileOpen::~CmdFileOpen()
{

}

void CmdFileOpen::Execute()
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



  // TODO OpenFileWindow.Open() etc
}
