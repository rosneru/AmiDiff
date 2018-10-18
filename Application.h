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
#include "TextViewWindow.h"
#include "Command.h"

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

  bool m_bExitRequested;
  AppScreen* m_pScreen;
  TextViewWindow* m_pLeftWin;
  TextViewWindow* m_pRightWin;

  AppMenu* m_pMenu;
  Command* m_pCmdOpenLeftFile;
  Command* m_pCmdOpenRightFile;
  Command* m_pCmdQuit;


  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
