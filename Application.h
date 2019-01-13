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
#include "ApplicationMenu.h"
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

  /**
   * Starts the application.
   *
   * When p_bDoNotAsk is set to true, the diff will be performed
   * immediately without waiting for the user to click the "Diff"
   * button in OpenFilesWindow. This only works if both files,
   * left and right are passed as argument.
   */
  bool Run(bool p_bDoNotAsk);

private:
  struct MsgPort* m_pMsgPortAllWindows;
  SimpleString m_PubScreenName;
  bool m_bExitRequested;

  AppScreen m_Screen;
  DiffWindow m_DiffWindow;
  AmigaDiffFacade m_DiffFacade;
  OpenFilesWindow m_OpenFilesWin;
  CmdPerformDiff m_CmdDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpenFilesWindow;
  ApplicationMenu m_Menu;


  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
