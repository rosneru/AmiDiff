#ifndef CMD_DIFF_H
#define CMD_DIFF_H

#include "DiffWorker.h"
#include "Command.h"

/**
 * Command for opening a window.
 *
 * On execute this command will perform the diff using the given
 * DiffWorker.
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class CmdDiff : public Command
{
public:
  CmdDiff(DiffWorker& p_DiffWorker);
  virtual ~CmdDiff();
  virtual void Execute() const;

private:
  DiffWorker& m_DiffWorker;
};

#endif // CMD_PERFORM_DIFF_H
