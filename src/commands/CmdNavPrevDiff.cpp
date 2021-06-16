#include "CmdNavPrevDiff.h"

CmdNavPrevDiff::CmdNavPrevDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               const DiffWorker& diffWorker,
                               DiffWindow& diffWindow)
  : CmdNavigateDiffBase(pAllWindowsVector, diffWorker, diffWindow)
{

}

CmdNavPrevDiff::~CmdNavPrevDiff()
{

}

void CmdNavPrevDiff::Execute(struct Window* pActiveWindow)
{
  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  if(pLeftTextArea == NULL)
  {
    return;
  }

  // Get lineId of next diff block
  long lineId = -1;
  if(m_FormerLineId < 0)
  {
    lineId = getNextResult(pLeftTextArea->getY() 
                           + pLeftTextArea->getMaxVisibleLines());
  }
  else if(pLeftTextArea->isLineVisible(m_FormerLineId))
  {
    // The former result is currently displayed. So getting the 
    // next result after that former result, not from the window 
    // top line.
    lineId = getPrevResult();
  }
  else
  {
    lineId = getPrevResult(pLeftTextArea->getY());
  }

  if(lineId < 0)
  {
    signalNoResultFound();
    return;
  }

  m_FormerLineId = lineId;

  // Scroll to this lineId
  m_DiffWindow.scrollToPage(0, lineId, 1, 1);
}
