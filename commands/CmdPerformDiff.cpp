#include <clib/dos_protos.h>
#include "AslFileRequest.h"
#include "DosFile.h"
#include "TextDocument.h"
#include "CmdPerformDiff.h"


CmdPerformDiff::CmdPerformDiff(AmigaDiffFacade& p_DiffFacade)
  : m_DiffFacade(p_DiffFacade)
{

}

CmdPerformDiff::~CmdPerformDiff()
{

}

void CmdPerformDiff::Execute()
{
  if(m_DiffFacade.Diff() == true)
  {
    
  }
}
