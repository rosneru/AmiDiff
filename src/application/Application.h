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
#include "ADiffViewPens.h"
#include "ADiffViewPorts.h"
#include "ADiffViewSettings.h"
#include "MenuFilesWindow.h"
#include "MenuDiffWindow.h"
#include "OpenClonedWorkbenchScreen.h"
#include "OpenJoinedPublicScreen.h"
#include "CmdRequester.h"
#include "CmdCloseWindow.h"
#include "CmdOpenWindow.h"
#include "CmdDiff.h"
#include "CmdNavNextDiff.h"
#include "CmdNavPrevDiff.h"
#include "CmdQuit.h"
#include "DiffWindow.h"
#include "DiffWorker.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
#include "SearchWindow.h"

class Application
{
public:
  Application(ScreenBase& screen, 
              ADiffViewArgs& args, 
              ADiffViewSettings& settings);
  virtual ~Application();

  void Run();

private:
  ScreenBase& m_Screen;
  ADiffViewArgs& m_Args;
  ADiffViewSettings& m_Settings;

  std::string m_LeftFilePath;
  std::string m_RightFilePath;

  bool m_IsCancelRequested;
  bool m_IsExitRequested;
  bool m_IsExitAllowed;
  bool m_IsAppWindow;
  bool m_IsAppIcon;

  ADiffViewPens m_Pens;
  MenuDiffWindow m_DiffWindowMenu;
  MenuFilesWindow m_FilesWindowMenu;
  ADiffViewPorts m_Ports;
  DiffWorker m_DiffWorker;
  DiffWindow m_DiffWindow;
  FilesWindow m_FilesWindow;
  ProgressWindow m_ProgressWindow;
  SearchWindow m_SearchWindow;
  CmdDiff m_CmdDiff;
  CmdNavNextDiff m_CmdNavNextDiff;
  CmdNavPrevDiff m_CmdNavPrevDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpenFilesWindow;
  CmdOpenWindow m_CmdOpenSearchWindow;
  CmdCloseWindow m_CmdCloseFilesWindow;
  CmdRequester m_CmdAboutRequester;
  std::string m_AboutMsg;

  std::vector<WindowBase*> m_AllWindowsList;

  struct DiskObject* m_Icon;
  struct AppIcon* m_pAppIcon;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();

  void handleAppWindowMessages();
  void handleProgressMessages();
  void handleIdcmpMessages();
};


#endif
