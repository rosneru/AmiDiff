#include "CmdDiff.h"

CmdDiff::CmdDiff(DiffWorker& diffWorker)
  : m_DiffWorker(diffWorker)
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
