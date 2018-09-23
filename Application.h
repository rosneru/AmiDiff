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
  AppScreen* m_pScreen;
  DiffWindow* m_pLeftWin;
  DiffWindow* m_pRightWin;
  AppMenu* m_pMenu;

  /**
   * Handling messages from Intuition
   */
  void intuiEventLoop();
};


#endif
