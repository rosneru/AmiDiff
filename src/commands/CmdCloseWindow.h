#ifndef CMD_CLOSE_WINDOW_H
#define CMD_CLOSE_WINDOW_H

#include "AMenu.h"
#include "CmdOpenWindow.h"
#include "CommandBase.h"

/**
 * Command for closing a window.
 *
 * On Execute this will close the provided window.
 *
 * @author Uwe Rosner
 * @date 06/07/2020
 */
class CmdCloseWindow : public CommandBase
{
public:
  CmdCloseWindow(Array<WindowBase*>& windowArray,
                 const CmdOpenWindow& openWindowCommand,
                 WindowBase& window);

  virtual ~CmdCloseWindow();
  virtual void Execute(struct Window* pActiveWindow);

private:
  const CmdOpenWindow& m_OpenWindowCommand;
  WindowBase& m_Window;
};

#endif