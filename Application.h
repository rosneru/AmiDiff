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
#include "DiffWindow.h"
#include "Command.h"

class Application
{
public:
  Application();
  ~Application();

  /**
   * Deletes all created objects and frees all allocated memory
   */
  void Dispose();

  bool Run();

private:
  bool m_bExitRequested;
  AppScreen* m_pScreen;
  DiffWindow* m_pLeftWin;
  DiffWindow* m_pRightWin;

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
