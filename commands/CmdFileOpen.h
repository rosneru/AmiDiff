#ifndef CMD_FILEOPEN_H
#define CMD_FILEOPEN_H

#include "SimpleString.h"
#include "Command.h"
#include "Document.h"

/**
 * Command for opening, reading and displaying an input file. Its
 * second parameter is a DiffDocument on which the open command will
 * be performed.
 *
 * On Execute of this command the ReadFile() method of the DiffDocument
 * will be called.
 *
 * @author Uwe Rosner
 * @date 24/09/2018
 */
class CmdFileOpen : public Command
{
public:
  CmdFileOpen(SimpleString p_Name, Document& p_Document);
  virtual ~CmdFileOpen();
  virtual void Execute();

private:
  Document& m_Document;
};

#endif // CMD_FILEOPEN_H
