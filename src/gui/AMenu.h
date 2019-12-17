#ifndef APP_MENU_H
#define APP_MENU_H

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include "AScreen.h"

/**
 * Class for the application menu. It can be created with a GadTools
 * struct NewMenu and the attached to an Intuition window.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class AMenu
{
public:
  AMenu(AScreen& screen);
  ~AMenu();

  /**
   * Creating and outlaying the menu
   */
  bool Create(struct NewMenu* pMenuDefinition);

  /**
   * Freeing the menu and all obtained resources
   */
  void Dispose();

  /**
   * Wire the menu strip to a window. Can be done for multiple windows.
   */
  bool AttachToWindow(struct Window* pWindow);

  /**
   * Update a previosly set attached menu. Call this if an checkbox has
   * changed or if a item has been enabled or disabled.
   *
   */
  bool UpdateInWindow(struct Window* pWindow);

  /**
   *  Remove menu from window. Call this at least before closing the
   *  window.
   */
  void DetachFromWindow(struct Window* pWindow);

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
   * @param pWindow
   * Pointer to an intuition window the menu is associated with
   *
   * @parem pUserDataMenuItemToDisable
   * Pointer by which value the userdata field of the menu items are
   * browsed
   *
   */
  void DisableMenuItem(struct Window* pWindow,
                       APTR pUserDataMenuItemToDisable);


  /**
   * Enables a previously disabled menu item.
   *
   * @param pWindow
   * Pointer to an intuition window the menu is associated with
   *
   * @parem pMenuItemToEnable
   * Pointer to the userdata field of the menu item to enable
   */
  void EnableMenuItem(struct Window* pWindow,
                      APTR pUserDataMenuItemToEnable);

private:
  AScreen& m_Screen;
  struct Menu* m_pMenu;


  /**
   * Browses all menu items trying to find the item with the given
   * userdata
   *
   * @param
   *
   * NOTE At the moment subitems are skipped
   */
  struct MenuItem* findItemByUserData(APTR pUserDataToFind,
                                      WORD& foundMenuNumber);
};

#endif