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
#include "DiffWindow.h"

class Application
{
public:
  /**
   * Creates the application using the given message port for the 
   * windows messages. If an non-empty string for p_PubScreenName is
   * provided, the 
   */
  Application(struct MsgPort* p_pMsgPortAllWindows, 
    const SimpleString& p_PubScreenName);

  ~Application();

  void SetLeftFilePath(const SimpleString& p_LeftFilePath);
  void SetRightFilePath(const SimpleString& p_RightFilePath);

  bool Run();

private:
  struct MsgPort* m_pMsgPortAllWindows;
  bool m_bExitRequested;

  AppScreen m_Screen;
  DiffWindow m_DiffWindow;
  AmigaDiffFacade m_DiffFacade;
  OpenFilesWindow m_OpenFilesWin;
  CmdPerformDiff m_CmdDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpenFilesWindow;
  AppMenu m_Menu;


  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
