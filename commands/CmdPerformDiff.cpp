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
