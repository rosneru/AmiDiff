#ifndef CMD_ABOUT_H
#define CMD_ABOUT_H

#include "AMenu.h"
#include "ScreenBase.h"
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
  CmdAbout(ScreenBase& screen,
           AMenu& menu,
           const char* pVersTag);

  virtual ~CmdAbout();
  virtual void Execute() const;

  SimpleString m_AboutMsg;

private:
  ScreenBase& m_Screen;
  AMenu& m_Menu;
};

#endif // CMD_ABOUT_H
