#ifndef CMD_PERFORM_DIFF_H
#define CMD_PERFORM_DIFF_H

#include "Command.h"
#include "DiffFacade.h"

/**
 * Command for opening a window.
 *
 * On Execute this will perform the diff with the given DiffFacade
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class CmdPerformDiff : public Command
{
public:
  CmdPerformDiff(DiffFacade& p_Facade);
  virtual ~CmdPerformDiff();
  virtual void Execute();

private:
  DiffFacade&  m_Facade;
};

#endif // CMD_PERFORM_DIFF_H
