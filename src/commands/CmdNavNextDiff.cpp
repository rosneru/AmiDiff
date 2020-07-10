#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(Array<WindowBase*>* pAllWindowsArray,
                               DiffWindow& diffWindow)
  : CommandBase(pAllWindowsArray),
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
