#include "CmdNavigatePrevDiff.h"

CmdNavigatePrevDiff::CmdNavigatePrevDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               const DiffWorker& diffWorker,
                               DiffWindow& diffWindow)
  : CmdNavigateBase(pAllWindowsVector, diffWorker, diffWindow)
{

}

CmdNavigatePrevDiff::~CmdNavigatePrevDiff()
{

}

void CmdNavigatePrevDiff::Execute(struct Window* pActiveWindow)
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
      lineId = getPrevResult(pLeftTextArea->getY() 
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
    hasScrolled = m_DiffWindow.scrollToPage(0, lineId, 1, 1);
  } 
  while (hasScrolled == false); // Repeatedly getting the next diff block
                                // until the view scrolled once.
}
