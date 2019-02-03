#ifndef CMD_PERFORM_DIFF_H
#define CMD_PERFORM_DIFF_H

#include "AmigaDiffFacade.h"
#include "Command.h"

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
  CmdPerformDiff(AmigaDiffFacade& p_DiffFacade);
  virtual ~CmdPerformDiff();
  virtual void Execute() const;

private:
  AmigaDiffFacade&  m_DiffFacade;
};

#endif // CMD_PERFORM_DIFF_H
