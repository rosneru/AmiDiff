#ifndef CMD_FILEOPEN_H
#define CMD_FILEOPEN_H

#include "SimpleString.h"
#include "Command.h"
#include "DiffDocument.h"

class CmdFileOpen : public Command
{
public:
  CmdFileOpen(SimpleString p_Name, DiffDocument& p_pDocument);
  virtual ~CmdFileOpen();
  virtual void Execute();

private:
  DiffDocument& m_Document;
};

#endif // CMD_FILEOPEN_H
