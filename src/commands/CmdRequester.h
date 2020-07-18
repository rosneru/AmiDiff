#ifndef CMD_REQUESTER_H
#define CMD_REQUESTER_H

#include "AMenu.h"
#include "CommandBase.h"
#include "SimpleString.h"


/**
 * This command opens a message box informing the user and allows to
 * select one or more buttons.
 *
 * @author Uwe Rosner
 * @date 20/02/2019
 */
class CmdRequester : public CommandBase
{
public:
  CmdRequester(std::vector<WindowBase*>* pAllWindowsVector,
               const SimpleString& message,
               const char* pTitle,
               const char* pButtons);

  virtual ~CmdRequester();
  virtual void Execute(struct Window* pActiveWindow);

private:
  const SimpleString& m_Message;
  const char* m_pTitle;
  const char* m_pButtons;

  long showRequester(struct Window* pActiveWindow);
};

#endif
