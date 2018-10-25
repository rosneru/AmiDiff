/* Application.h                                                       */
/*-------------------------------------------------------------------*/
/* Main application class                                            */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPLICATION
#define APPLICATION

#include "AppMenu.h"
#include "AppScreen.h"
#include "Command.h"
#include "OpenFilesWindow.h"
#include "SimpleString.h"
#include "TextWindow.h"

class Application
{
public:
  Application(int argc, char **argv);
  ~Application();

  /**
   * Deletes all created objects and frees all allocated memory
   */
  void Dispose();

  bool Run();

private:
  int m_Argc;
  char** m_Argv;

  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;

  bool m_bExitRequested;
  
  AppScreen* m_pScreen;
  OpenFilesWindow* m_pOpenFilesWin;
  TextWindow* m_pLeftWin;
  TextWindow* m_pRightWin;

  AppMenu* m_pMenu;
  Command* m_pCmdOpenFilesWindow;
  Command* m_pCmdOpenRightFile;
  Command* m_pCmdQuit;


  /**
   * Calculating the signal mask for intuiEventLoop in dependency of
   * which window is open and available
   */
  uint32 signalMask();

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
