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
#include "AMenu.h"
#include "ClonedWorkbenchScreen.h"
#include "JoinedPublicScreen.h"
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
  struct MsgPort* m_pMsgPortAppWindow;

  SimpleString m_ErrorMsg;

  int m_NumWindowsOpen;
  bool m_bCancelRequested;
  bool m_bExitRequested;
  bool m_bExitAllowed;

  /**
   * IMPORTANT:
   *
   * The order of the following items is IMPORTANT - Because the
   * destructor calls will be in reverse order.
   *
   */

  DiffWorker m_DiffWorker;
  ClonedWorkbenchScreen m_ClonedWorkbenchScreen;
  JoinedPublicScreen m_JoinedPublicScreen;
  JoinedPublicScreen m_WorkbenchPublicScreen;
  ScreenBase* m_pDiffWindowScreen;
  ScreenBase* m_pFilesWindowScreen;
  AMenu m_Menu;
  AMenu m_MenuDiffWindow;
  DiffWindow m_DiffWindow;
  FilesWindow m_FilesWindow;
  ProgressWindow m_ProgressWindow;
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

  void handleAppWindowMessages();
  void handleProgressMessages();
  void handleIdcmpMessages();
};


#endif
