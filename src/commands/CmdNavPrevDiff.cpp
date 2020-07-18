#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
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

