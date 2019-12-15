#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(DiffWindow& diffWindow)
  : m_DiffWindow(diffWindow)
{

}

CmdNavNextDiff::~CmdNavNextDiff()
{

}

void CmdNavNextDiff::Execute() const
{
  m_DiffWindow.NavigateToNextDiff();
}
