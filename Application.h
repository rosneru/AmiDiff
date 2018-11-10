/* Application.h                                                       */
/*-------------------------------------------------------------------*/
/* Main application class                                            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPLICATION
#define APPLICATION

#include <exec/ports.h>

#include "AmigaDiffFacade.h"
#include "AppMenu.h"
#include "AppScreen.h"
#include "CmdOpenWindow.h"
#include "CmdPerformDiff.h"
#include "CmdQuit.h"
#include "AmigaDiffFacade.h"
#include "OpenFilesWindow.h"
#include "SimpleString.h"
#include "TextWindow.h"

class Application
{
public:
  Application(int argc, char **argv, struct MsgPort* p_pMsgPortAllWindows);
  ~Application();

  bool Run();

private:
  struct MsgPort* m_pMsgPortAllWindows;
  int m_Argc;
  char** m_Argv;

  bool m_bExitRequested;

  AppScreen m_Screen;
  OpenFilesWindow m_OpenFilesWin;
  TextWindow m_LeftWin;
  TextWindow m_RightWin;
  AppMenu m_Menu;

  CmdOpenWindow m_CmdOpenFilesWindow;
  CmdPerformDiff m_CmdDiff;
  CmdQuit m_CmdQuit;

  AmigaDiffFacade m_DiffFacade;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
