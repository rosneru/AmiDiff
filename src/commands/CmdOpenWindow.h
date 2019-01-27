#ifndef CMD_OPEN_WINDOW_H
#define CMD_OPEN_WINDOW_H

#include "ApplicationMenu.h"
#include "Command.h"
#include "WindowBase.h"

/**
 * Command for opening a window.
 *
 * On Execute this will open the provided window and attach the provided
 * menu to it
 *
 * @author Uwe Rosner
 * @date 24/10/2018
 */
class CmdOpenWindow : public Command
{
public:
  CmdOpenWindow(WindowBase& p_Window);
  virtual ~CmdOpenWindow();
  virtual void Execute();

private:
  WindowBase& m_Window;
};

#endif // CMD_OPEN_WINDOW_H
