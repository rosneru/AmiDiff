#ifndef CMD_NAVIGATE_NEXT_DIFF_H
#define CMD_NAVIGATE_NEXT_DIFF_H

#include "DiffWindow.h"
#include "CmdNavigateDiffBase.h"

/**
 * Command for displaying the next difference in DiffWindow.
 *
 * @author Uwe Rosner
 * @date 14/12/2019
 */
class CmdNavigateNextDiff : public CmdNavigateDiffBase
{
public:
  CmdNavigateNextDiff(std::vector<WindowBase*>* pAllWindowsVector,
                 const DiffWorker& diffWorker,
                 DiffWindow& diffWindow);

  virtual ~CmdNavigateNextDiff();
  virtual void Execute(struct Window* pActiveWindow);
};

#endif
