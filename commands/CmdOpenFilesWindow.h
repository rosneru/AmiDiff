#ifndef CMD_FILEOPEN_H
#define CMD_FILEOPEN_H

#include "SimpleString.h"
#include "Command.h"
#include "OpenFilesWindow.h"
#include "TextDocument.h"
#include "TextWindow.h"

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
  CmdFileOpen(SimpleString p_Name, TextDocument& p_TextDocument, TextWindow* p_pWindow);
  virtual ~CmdFileOpen();
  virtual void Execute();

private:
  OpenFilesWindow m_OpenFilesWindow;
  TextDocument& m_TextDocument;
  TextWindow* m_pWindow;
};

#endif // CMD_FILEOPEN_H
