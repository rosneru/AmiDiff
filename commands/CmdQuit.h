#ifndef CMD_QUIT_H
#define CMD_QUIT_H

#include "Command.h"

class CmdQuit : public Command
{
public:
  virtual ~CmdQuit();
  CmdQuit(SimpleString p_Name, bool& p_bExitRequested);
  virtual void Execute();

private:
  bool& m_bExitRequested;
};

#endif // CMD_QUIT_H
