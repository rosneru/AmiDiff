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
  Command(SimpleString p_Name);
  SimpleString m_Name;

};

#endif // COMMAND_H
