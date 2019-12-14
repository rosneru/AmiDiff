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

#include "ADiffViewArgs.h"
#include "ADiffViewSettings.h"
#include "ApplicationMenu.h"
#include "AppScreen.h"
#include "CmdAbout.h"
#include "CmdOpenWindow.h"
#include "CmdDiff.h"
#include "CmdNavNextDiff.h"
#include "CmdNavPrevDiff.h"
#include "CmdQuit.h"
#include "DiffWorker.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
#include "SimpleString.h"
#include "DiffWindow.h"

class Application
{
public:
  Application(ADiffViewArgs& args);
  ~Application();

  bool Run();
  SimpleString& ErrorMsg();

private:
  ADiffViewArgs& m_Args;
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;

  ADiffViewSettings m_Settings;

  struct MsgPort* m_pMsgPortIDCMP;
  struct MsgPort* m_pMsgPortProgress;

  SimpleString m_ErrorMsg;

  int m_NumWindowsOpen;
  bool m_bCancelRequested;
  bool m_bExitRequested;
  bool m_bExitAllowed;

  /**
   * IMPORTANT NOTE
   *
   * The order of the following items is IMPORTANT - Because the
   * destructor calls will be in reverse order.
   *
   * By Starting with m_Screen followed by m_Menu it is ensured that
   * the next-to-last destructor call is the m_Menu destructor with
   * the destructor call of m_Screen followed - which should be the
   * final destroyed object.
   *
   */

  AppScreen m_Screen;
  ApplicationMenu m_Menu;
  ApplicationMenu m_MenuDiffWindow;
  DiffWindow m_DiffWindow;
  FilesWindow m_FilesWindow;
  ProgressWindow m_ProgressWindow;
  DiffWorker m_DiffWorker;
  CmdDiff m_CmdDiff;
  CmdNavNextDiff m_CmdNavNextDiff;
  CmdNavPrevDiff m_CmdNavPrevDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpen;
  CmdAbout m_CmdAbout;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
