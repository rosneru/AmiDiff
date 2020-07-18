/* Application.h                                                       */
/*-------------------------------------------------------------------*/
/* Main application class                                            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPLICATION
#define APPLICATION

#include <vector>

#include <exec/ports.h>

#include "ADiffViewArgs.h"
#include "ADiffViewSettings.h"
#include "AMenu.h"
#include "OpenClonedWorkbenchScreen.h"
#include "OpenJoinedPublicScreen.h"
#include "CmdRequester.h"
#include "CmdCloseWindow.h"
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
  Application(ScreenBase* pScreenBase, 
              ADiffViewArgs& args, 
              ADiffViewSettings& settings);
  virtual ~Application();

  bool Run();
  SimpleString& ErrorMsg();

private:
  ADiffViewArgs& m_Args;
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;

  ADiffViewSettings& m_Settings;

  struct MsgPort* m_pMsgPortIDCMP;
  struct MsgPort* m_pMsgPortProgress;
  struct MsgPort* m_pMsgPortAppWindow;

  SimpleString m_ErrorMsg;

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
  ScreenBase* m_pScreenBase;
  AMenu m_FilesWindowMenu;
  AMenu m_DiffWindowMenu;
  DiffWindow m_DiffWindow;
  FilesWindow m_FilesWindow;
  ProgressWindow m_ProgressWindow;
  CmdDiff m_CmdDiff;
  CmdNavNextDiff m_CmdNavNextDiff;
  CmdNavPrevDiff m_CmdNavPrevDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpenFilesWindow;
  CmdCloseWindow m_CmdCloseFilesWindow;
  CmdRequester m_CmdAboutRequester;
  SimpleString m_AboutMsg;

  std::vector<WindowBase*> m_WindowArray;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();

  void handleAppWindowMessages();
  void handleProgressMessages();
  void handleIdcmpMessages();
};


#endif
