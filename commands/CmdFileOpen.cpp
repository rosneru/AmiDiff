#include "CmdFileOpen.h"


CmdFileOpen::CmdFileOpen(SimpleString p_Name, Document& p_Document)
  : m_Document(p_Document), 
    Command(p_Name)
{

}

CmdFileOpen::~CmdFileOpen()
{

}

void CmdFileOpen::Execute()
{
  //m_Document.ReadFile();
  // TODO OpenFileWindow.Open() etc
}
