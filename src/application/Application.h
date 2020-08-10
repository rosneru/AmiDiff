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

  struct NewMenu m_MenuDefFilesWindow[6] = 
  {
    { NM_TITLE,   "Project",                 0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",              "O", 0, 0, &m_CmdOpenFilesWindow },
    {   NM_ITEM,    "About...",              0 , 0, 0, &m_CmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,             0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                 "Q", 0, 0, &m_CmdQuit },
    { NM_END,     NULL,                      0 , 0, 0, 0 },
  };

  struct NewMenu m_MenuDefDiffWindow[9] =
  {
    { NM_TITLE,   "Project",                 0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",              "O", 0, 0, &m_CmdOpenFilesWindow },
    {   NM_ITEM,    "About...",              0 , 0, 0, &m_CmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,             0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                 "Q", 0, 0, &m_CmdQuit },
    { NM_TITLE,   "Navigate",                0 , 0, 0, 0 },
    {   NM_ITEM,    "Previous difference",  "P", 0, 0, &m_CmdNavPrevDiff },
    {   NM_ITEM,    "Next difference",      "N", 0, 0, &m_CmdNavNextDiff },
    { NM_END,     NULL,                      0 , 0, 0, 0 },
  };

  AMenu m_FilesWindowMenu;
  AMenu m_DiffWindowMenu;

  bool m_IsCancelRequested;
  bool m_IsExitRequested;
  bool m_IsExitAllowed;
  bool m_IsAppWindow;
  bool m_IsAppIcon;

  /**
   * IMPORTANT:
   *
   * The order of the following items is IMPORTANT - Because the
   * destructor calls will be in reverse order.
   *
   */

  ADiffViewPorts m_Ports;
  DiffWorker m_DiffWorker;
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
