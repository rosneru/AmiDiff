#ifndef CMD_NAV_NEXT_DIFF_H
#define CMD_NAV_NEXT_DIFF_H

#include "DiffWindow.h"
#include "CommandBase.h"

/**
 * Command for displaying the next difference in DiffWindow.
 *
 * @author Uwe Rosner
 * @date 14/12/2019
 */
class CmdNavNextDiff : public CommandBase
{
public:
  CmdNavNextDiff(Array<WindowBase*>* pAllWindowsArray,
                 DiffWindow& diffWindow);

  virtual ~CmdNavNextDiff();
  virtual void Execute(struct Window* pActiveWindow);

private:
  DiffWindow& m_DiffWindow;
};

#endif // CMD_PERFORM_DIFF_H
