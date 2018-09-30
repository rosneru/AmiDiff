#include "CmdFileOpen.h"


CmdFileOpen::CmdFileOpen(SimpleString p_Name, DiffDocument& p_pDocument)
  : Command(p_Name),
  m_Document(p_pDocument)
{

}

CmdFileOpen::~CmdFileOpen()
{

}

void CmdFileOpen::Execute()
{
  m_Document.ReadFile();
}
