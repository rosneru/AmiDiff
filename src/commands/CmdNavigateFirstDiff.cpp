#include "CmdNavigateFirstDiff.h"

CmdNavigateFirstDiff::CmdNavigateFirstDiff(std::vector<WindowBase*>* pAllWindowsVector,
                               const DiffWorker& diffWorker,
                               DiffWindow& diffWindow)
  : CmdNavigateBase(pAllWindowsVector, diffWorker, diffWindow)
{

}

CmdNavigateFirstDiff::~CmdNavigateFirstDiff()
{

}

void CmdNavigateFirstDiff::Execute(struct Window* pActiveWindow)
{
  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  if(pLeftTextArea == NULL)
  {
    return;
  }

  // Get lineId of first diff block (The next one starting from line 0).
  long lineId = getNextResult(0);
  if(lineId < 0)
  {
    // Should not be possible, a first diff block *should* always exist
    signalNoResultFound();
    return;
  }

  m_FormerLineId = lineId;
  m_DiffWindow.scrollToPage(0, lineId, 1, 1);

}
