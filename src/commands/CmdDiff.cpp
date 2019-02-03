#include "CmdDiff.h"

CmdDiff::CmdDiff(DiffWorker& p_DiffWorker)
  : m_DiffWorker(p_DiffWorker)
{

}

CmdDiff::~CmdDiff()
{

}

void CmdDiff::Execute() const
{
  // Run() starts the diff as asynchronous background process
  m_DiffWorker.Run();
}
