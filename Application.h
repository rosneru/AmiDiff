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

  /**
   * NOTE The order of the following items is IMPORTANT. Because the
   *      destructor calls will be in reverse order.
   *
   *      By Starting with m_Screen followed by m_Menu it is ensured
   *      that the next-to-last destructor call is the m_Menu
   *      destructor followed by the destructor call of m_Screen which
   *      should be the final destroyed object.
   */

  AppScreen m_Screen;
  ApplicationMenu m_Menu;
  DiffWindow m_DiffWindow;
  AmigaDiffFacade m_DiffFacade;
  OpenFilesWindow m_OpenFilesWin;
  CmdPerformDiff m_CmdDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpenFilesWindow;


  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
