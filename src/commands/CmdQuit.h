#ifndef CMD_QUIT_H
#define CMD_QUIT_H

#include "Command.h"

/**
 * Command for exiting the application. It takes an boolean reference
 * as second parameter. When Execute is called this reference variable
 * will be set to true so that the application in its loop can react
 * and quit.
 *
 * @author Uwe Rosner
 * @date 24/09/2018
 */
class CmdQuit : public Command
{
public:
  virtual ~CmdQuit();
  CmdQuit(bool& p_bExitAllowed, bool& p_bExitRequested);
  virtual void Execute() const;

private:
  bool& m_bExitAllowed;
  bool& m_bExitRequested;
};

#endif // CMD_QUIT_H
