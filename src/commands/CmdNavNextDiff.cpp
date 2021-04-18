#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
    m_DiffWindow(diffWindow)
{

}

CmdNavNextDiff::~CmdNavNextDiff()
{

}

void CmdNavNextDiff::Execute(struct Window* pActiveWindow)
{
  m_DiffWindow.navigateToNextDiff();
}
