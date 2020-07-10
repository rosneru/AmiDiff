#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(Array<WindowBase*>* pAllWindowsArray,
                               DiffWindow& diffWindow)
  : CommandBase(pAllWindowsArray),
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

