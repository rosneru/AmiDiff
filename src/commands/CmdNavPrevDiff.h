#ifndef CMD_NAV_PREV_DIFF_H
#define CMD_NAV_PREV_DIFF_H

#include "DiffWindow.h"
#include "Command.h"

/**
 * Command for displaying the previous difference in DiffWindow.
 *
 * @author Uwe Rosner
 * @date 14/12/2019
 */
class CmdNavPrevDiff : public Command
{
public:
  CmdNavPrevDiff(DiffWindow& diffWindow);
  virtual ~CmdNavPrevDiff();
  virtual void Execute(struct Window* pActiveWindow) const;

private:
  DiffWindow& m_DiffWindow;
};

#endif // CMD_PERFORM_DIFF_H
