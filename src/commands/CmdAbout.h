#ifndef CMD_ABOUT_H
#define CMD_ABOUT_H

#include "AppScreen.h"
#include "ApplicationMenu.h"
#include "Command.h"
#include "SimpleString.h"

/**
 * This command opens a message box informing the user about the author
 * of the program.
 *
 * @author Uwe Rosner
 * @date 20/02/2019
 */
class CmdAbout : public Command
{
public:
  CmdAbout(AppScreen& p_Screen, ApplicationMenu& p_Menu);
  virtual ~CmdAbout();
  virtual void Execute() const;

  SimpleString m_AboutMsg;

private:
  AppScreen& m_Screen;
  ApplicationMenu& m_Menu;
};

#endif // CMD_ABOUT_H
