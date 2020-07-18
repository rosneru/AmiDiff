#ifndef CMD_NAV_PREV_DIFF_H
#define CMD_NAV_PREV_DIFF_H

#include "DiffWindow.h"
#include "CommandBase.h"

/**
 * Command for displaying the previous difference in DiffWindow.
 *
 * @author Uwe Rosner
 * @date 14/12/2019
 */
class CmdNavPrevDiff : public CommandBase
{
public:
  CmdNavPrevDiff(std::vector<WindowBase*>* pAllWindowsArray,
                 DiffWindow& diffWindow);

  virtual ~CmdNavPrevDiff();
  virtual void Execute(struct Window* pActiveWindow);

private:
  DiffWindow& m_DiffWindow;
};

#endif // CMD_PERFORM_DIFF_H
