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

#include "ApplicationMenu.h"
#include "AppScreen.h"
#include "CmdAbout.h"
#include "CmdOpenWindow.h"
#include "CmdDiff.h"
#include "CmdQuit.h"
#include "DiffWorker.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
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
  Application(struct MsgPort* p_pMsgPortIDCMP,
              struct MsgPort* p_pMsgPortProgress);

  ~Application();

  void SetLeftFilePath(const SimpleString& p_LeftFilePath);
  void SetRightFilePath(const SimpleString& p_RightFilePath);
  void SetPubScreenName(const SimpleString& p_PubScreenName);

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
  struct MsgPort* m_pMsgPortIDCMP;
  struct MsgPort* m_pMsgPortProgress;
  SimpleString m_PubScreenName;
  int m_NumWindowsOpen;
  bool m_bCancelRequested;
  bool m_bExitRequested;
  bool m_bExitAllowed;

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
  FilesWindow m_FilesWindow;
  ProgressWindow m_ProgressWindow;
  DiffWorker m_DiffWorker;
  CmdAbout m_CmdAbout;
  CmdDiff m_CmdDiff;
  CmdQuit m_CmdQuit;
  CmdOpenWindow m_CmdOpen;
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
