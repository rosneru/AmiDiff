#include "CmdDiff.h"

CmdDiff::CmdDiff(Array<WindowBase*>* pAllWindowsArray,
                 DiffWorker& diffWorker)
  : CommandBase(pAllWindowsArray),
    m_DiffWorker(diffWorker)
{

}

CmdDiff::~CmdDiff()
{

}

void CmdDiff::Execute(struct Window* pActiveWindow)
{
  // Run() starts the diff as asynchronous background process
  m_DiffWorker.Run();
}
