#ifndef COMMAND_H
#define COMMAND_H

#include "SimpleString.h"

class Command
{
public:
  virtual ~Command();
  virtual void Execute() = 0;
  virtual SimpleString Name();

protected:
  SimpleString m_Name;
  Command(SimpleString p_Name);

};

#endif // COMMAND_H
