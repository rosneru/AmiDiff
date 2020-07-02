#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(Array<WindowBase*>& windowArray,
                               DiffWindow& diffWindow)
  : CommandBase(windowArray),
    m_DiffWindow(diffWindow)
{

}

CmdNavPrevDiff::~CmdNavPrevDiff()
{

}

void CmdNavPrevDiff::Execute(struct Window* pActiveWindow)
{
  m_DiffWindow.NavigateToPrevDiff();
}

