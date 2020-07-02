#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(Array<WindowBase*>& windowArray,
                               DiffWindow& diffWindow)
  : CommandBase(windowArray),
    m_DiffWindow(diffWindow)
{

}

CmdNavNextDiff::~CmdNavNextDiff()
{

}

void CmdNavNextDiff::Execute(struct Window* pActiveWindow)
{
  m_DiffWindow.NavigateToNextDiff();
}
