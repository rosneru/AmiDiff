#ifndef CMD_NAVIGATE_DIFF_BASE
#define CMD_NAVIGATE_DIFF_BASE

#include "CommandBase.h"
#include "DiffWorker.h"
#include "DiffWindow.h"

/**
 * Base class for the Navigate-between-diff-blocks commands.
 * 
 * @author Uwe Rosner
 * @date 16/06/2021
 */
class CmdNavigateBase  : public CommandBase
{
public:
  CmdNavigateBase(std::vector<WindowBase*>* pAllWindowsVector,
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
