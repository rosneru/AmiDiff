#include "CmdNavNextDiff.h"

CmdNavNextDiff::CmdNavNextDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               const DiffWorker& diffWorker,
                               DiffWindow& diffWindow)
  : CmdNavigateDiffBase(pAllWindowsVector, diffWorker, diffWindow)
{

}

CmdNavNextDiff::~CmdNavNextDiff()
{

}

void CmdNavNextDiff::Execute(struct Window* pActiveWindow)
{
  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  if(pLeftTextArea == NULL)
  {
    return;
  }

  // Get lineId of next diff block
  long lineId = -1;
  bool hasScrolled = false;
  do
  {
    if(m_FormerLineId < 0)
    {
      lineId = getNextResult(pLeftTextArea->getY());
    }
    else if(pLeftTextArea->isLineVisible(m_FormerLineId))
    {
      // The former result is currently displayed. So getting the 
      // next result after that former result, not from the window 
      // top line.
      lineId = getNextResult();
    }
    else
    {
      lineId = getNextResult(pLeftTextArea->getY());
    }

    if(lineId < 0)
    {
      signalNoResultFound();
      return;
    }

    m_FormerLineId = lineId;
    hasScrolled = m_DiffWindow.scrollToPage(0, lineId, 1, 1);
  } 
  while (hasScrolled == false); // Repeatedly getting the next diff block
                                // until the view scrolled once.
}
