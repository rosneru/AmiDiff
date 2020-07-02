#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(DiffWindow& diffWindow)
  : m_DiffWindow(diffWindow)
{

}

CmdNavPrevDiff::~CmdNavPrevDiff()
{

}

void CmdNavPrevDiff::Execute(struct Window* pActiveWindow) const
{
  m_DiffWindow.NavigateToPrevDiff();
}

