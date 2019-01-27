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
  // Run() starts the diff as asynchronous background process
  m_DiffFacade.Run();
}
