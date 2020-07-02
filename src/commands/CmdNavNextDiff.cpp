#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(DiffWindow& diffWindow)
  : m_DiffWindow(diffWindow)
{

}

CmdNavNextDiff::~CmdNavNextDiff()
{

}

void CmdNavNextDiff::Execute(struct Window* pActiveWindow) const
{
  m_DiffWindow.NavigateToNextDiff();
}
