#ifndef CMD_ABOUT_H
#define CMD_ABOUT_H

#include "AMenu.h"
#include "CommandBase.h"
#include "SimpleString.h"


/**
 * This command opens a message box informing the user about the author
 * of the program.
 *
 * @author Uwe Rosner
 * @date 20/02/2019
 */
class CmdAbout : public CommandBase
{
public:
  CmdAbout(Array<WindowBase*>& windowArray,
           const char* pVersTag);

  virtual ~CmdAbout();
  virtual void Execute(struct Window* pActiveWindow);

  SimpleString m_AboutMsg;

private:
  long showRequester(struct Window* pActiveWindow);

};

#endif // CMD_ABOUT_H
