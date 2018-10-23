#ifndef APPMENU_H
#define APPMENU_H

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include "AppScreen.h"
#include "WindowBase.h"

/**
 * Class for the application menu. It can be created with a GadTools
 * struct NewMenu and the attached to an Intuition window.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class AppMenu
{
public:
  AppMenu(AppScreen* p_pScreen);
  ~AppMenu();

  /**
   * Creating and outlaying the menu
   */
  bool Create(struct NewMenu* p_pMenuDefinition);

  /**
   * Freeing the menu and all obtained resources
   */
  void Dispose();

  /**
   * Wire the menu strip to a window. Can be done for multiple windows.
   */
  bool AttachToWindow(WindowBase* p_pWindow);

  /**
   * Gets the intuition menu structure or NULL if menu is not open
   */
  struct Menu* IntuiMenu();

private:
  AppScreen* m_pScreen;
  struct Menu* m_pMenu;
};

#endif
