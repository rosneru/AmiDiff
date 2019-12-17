#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(DiffWindow& diffWindow)
  : m_DiffWindow(diffWindow)
{

}

CmdNavPrevDiff::~CmdNavPrevDiff()
{

}

void CmdNavPrevDiff::Execute() const
{
  m_DiffWindow.NavigateToPrevDiff();
}
