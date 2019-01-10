#ifndef APPLICATION_MENU_H
#define APPLICATION_MENU_H

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include "AppScreen.h"

/**
 * Class for the application menu. It can be created with a GadTools
 * struct NewMenu and the attached to an Intuition window.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class ApplicationMenu
{
public:
  ApplicationMenu(AppScreen& p_Screen);
  ~ApplicationMenu();

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
  bool AttachToWindow(struct Window* p_pWindow);

  /**
   * Update a previosly set attached menu. Call this if an checkbox has
   * changed or if a item has been enabled or disabled.
   *
   */
  bool UpdateInWindow(struct Window* p_pWindow);

  /**
   *  Remove menu from window. Call this at least before closing the
   *  window.
   */
  void DetachFromWindow(struct Window* p_pWindow);

  /**
   * Gets the intuition menu structure or NULL if menu is not open
   */
  struct Menu* IntuiMenu();

  /**
   * Browses the menu associated to the given window for an item which
   * contains the given pointer in field userdata. If such an menu item
   * is found, it is disabled.
   *
   * NOTE At the moment subitems are skipped and won't be disabled.
   *
   * @param p_pWindow
   * Pointer to an intuition window the menu is associated with
   *
   * @parem p_pUserDataMenuItemToDisable
   * Pointer by which value the userdata field of the menu items are
   * browsed
   *
   */
  void DisableMenuItem(struct Window* p_pWindow,
    APTR p_pUserDataMenuItemToDisable);


  /**
   * Enables a previously disabled menu item.
   *
   * @param p_pWindow
   * Pointer to an intuition window the menu is associated with
   *
   * @parem p_pMenuItemToEnable
   * Pointer to the userdata field of the menu item to enable
   */
  void EnableMenuItem(struct Window* p_pWindow,
    APTR p_pUserDataMenuItemToEnable);

private:
  AppScreen& m_Screen;
  struct Menu* m_pMenu;


  /**
   * Browses all menu items trying to find the item with the given
   * userdata
   *
   * @param
   *
   * NOTE At the moment subitems are skipped
   */
  struct MenuItem* findItemByUserData(APTR p_pUserDataToFind,
    WORD& p_FoundMenuNumber);
};

#endif
