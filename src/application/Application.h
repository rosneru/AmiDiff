/* Application.h                                                       */
/*-------------------------------------------------------------------*/
/* Main application class                                            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPLICATION
#define APPLICATION

#include <workbench/workbench.h>

#include <string>
#include <vector>

#include "ADiffViewArgs.h"
#include "ADiffViewPorts.h"
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
#include "DiffWindow.h"

class Application
{
public:
  Application(ScreenBase* pScreenBase, 
              ADiffViewArgs& args, 
              ADiffViewSettings& settings);
  virtual ~Application();

  void Run();

private:
  ADiffViewArgs& m_Args;
  std::string m_LeftFilePath;
  std::string m_RightFilePath;

  ADiffViewSettings& m_Settings;

  ADiffViewPorts m_Ports;

  bool m_bCancelRequested;
  bool m_bExitRequested;
  bool m_bExitAllowed;
  bool m_IsAppWindow;
  bool m_IsAppIcon;

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
  std::string m_AboutMsg;

  std::vector<WindowBase*> m_AllWindows;

  struct DiskObject* dobj;
  struct AppIcon* appicon;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();

  void handleAppWindowMessages();
  void handleProgressMessages();
  void handleIdcmpMessages();
};


#endif
