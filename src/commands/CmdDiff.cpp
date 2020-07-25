#include "CmdDiff.h"

CmdDiff::CmdDiff(std::vector<WindowBase*>* pAllWindowsVector,
                 DiffWorker& diffWorker)
  : CommandBase(pAllWindowsVector),
    m_DiffWorker(diffWorker)
{

}

CmdDiff::~CmdDiff()
{

}

void CmdDiff::Execute(struct Window* pActiveWindow)
{
  // Run() starts the diff as asynchronous background process
  m_DiffWorker.Diff();
}
