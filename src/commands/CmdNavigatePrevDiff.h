#ifndef CMD_NAV_PREV_DIFF_H
#define CMD_NAV_PREV_DIFF_H

#include "DiffWindow.h"
#include "CmdNavigateDiffBase.h"

/**
 * Command for displaying the previous difference in DiffWindow.
 *
 * @author Uwe Rosner
 * @date 14/12/2019
 */
class CmdNavigatePrevDiff : public CmdNavigateDiffBase
{
public:
  CmdNavigatePrevDiff(std::vector<WindowBase*>* pAllWindowsVector,
                 const DiffWorker& diffWorker,
                 DiffWindow& diffWindow);

  virtual ~CmdNavigatePrevDiff();
  virtual void Execute(struct Window* pActiveWindow);

};

#endif // CMD_PERFORM_DIFF_H
