#include "CmdNavigateLastDiff.h"

CmdNavigateLastDiff::CmdNavigateLastDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               const DiffWorker& diffWorker,
                               DiffWindow& diffWindow)
  : CmdNavigateBase(pAllWindowsVector, diffWorker, diffWindow)
{

}

CmdNavigateLastDiff::~CmdNavigateLastDiff()
{

}

void CmdNavigateLastDiff::Execute(struct Window* pActiveWindow)
{
  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  if((pLeftTextArea == NULL) || (m_pDiffDocument == NULL))
  {
    return;
  }

  // Get lineId of last diff block (The one before the document end).
  long lineId = getPrevResult(m_pDiffDocument->getNumLines() - 1);
  if(lineId < 0)
  {
    // Should not be possible, a first diff block *should* always exist
    signalNoResultFound();
    return;
  }

  m_FormerLineId = lineId;
  m_DiffWindow.scrollToPage(0, lineId, 1, 1);
}
