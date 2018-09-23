/* AppMenu.h                                                         */
/*-------------------------------------------------------------------*/
/* Class for the application menu                                    */
/*                                                                   */
/* Uwe Rosner                                                        */
/* Created on 23/09/2018                                             */
/*-------------------------------------------------------------------*/
#ifndef APPMENU
#define APPMENU

#include <intuition/intuition.h>
#include <intuition/screens.h>

class AppMenu
{
public:
  AppMenu(struct Screen* p_pScreen);
  ~AppMenu();

  /**
   * Creating and layouting the menu
   */
  bool Create();

  /**
   * Freeing the menu and all obtained resources
   */
  void Dispose();

  /**
   * Wire the menu strip to a window. Can be done for multiple windows.
   */
  bool BindToWindow(struct Window* p_pWindow);

  /**
   * Gets the intuition menu structure or NULL if menu is not open
   */
  struct Menu* Menu();

private:
  APTR* m_pVisualInfo;
  struct Menu* m_pMenu;
};


#endif
