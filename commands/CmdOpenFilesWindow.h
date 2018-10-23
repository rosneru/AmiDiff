#ifndef CMD_OPEN_FILES_WINDOW
#define CMD_OPEN_FILES_WINDOW

#include "Command.h"
#include "OpenFilesWindow.h"

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
class CmdOpenFilesWindow : public Command
{
public:
  CmdOpenFilesWindow(OpenFilesWindow& p_OpenFilesWindow);
  virtual ~CmdOpenFilesWindow();
  virtual void Execute();

private:
  OpenFilesWindow& m_OpenFilesWindow;
};

#endif // CMD_OPEN_FILES_WINDOW
