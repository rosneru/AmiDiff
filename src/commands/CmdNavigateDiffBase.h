#ifndef CMD_NAVIGATE_DIFF_BASE
#define CMD_NAVIGATE_DIFF_BASE

#include "CommandBase.h"
#include "DiffWorker.h"
#include "DiffWindow.h"

class CmdNavigateDiffBase  : public CommandBase
{
public:
  CmdNavigateDiffBase(std::vector<WindowBase*>* pAllWindowsVector,
                      const DiffWorker& diffWorker,
                      DiffWindow& diffWindow);

protected:
  const DiffWorker& m_DiffWorker;
  DiffWindow& m_DiffWindow;

  const DiffDocument* m_pDiffDocument;
  std::list<size_t>::const_iterator m_DiffIndicesIterator;

  long m_FormerLineId;

  long getPrevResult();
  long getPrevResult(size_t startLineId);
  long getNextResult();
  long getNextResult(size_t startLineId);

  void signalNoResultFound();

private:
  void applyDocumentChanged();
};

#endif
