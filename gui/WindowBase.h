#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#include <exec/ports.h>
#include <intuition/screens.h>
#include "AppMenu.h"
#include "AppScreen.h"
#include "SimpleString.h"

/**
 * Abstract base class for all other windows
 *
 *
 * @author Uwe Rosner
 * @date 19/10/2018
 */
class WindowBase
{
public:
  /**
   * Used to define an initial position of the window on the screen
   */
  enum InitialWindowPosition
  {
    IWP_Center,       // Center of the screen
    IWP_Left,         // Left half of the screen
    IWP_Right,        // Right half of the screen
    IWP_FixedCenter,  // Center of the screen; window not moveable
    IWP_FixedLeft,    // Left half of the screen; window not moveable
    IWP_FixedRight    // Right half of the screen; window  not moveable
  };

  /**
   * Base class method for opening the window. Derived classes should
   * call this in their Open() method after the window has opened.
   *
   * Sets the menu strip to the window if one had been provided. If the
   * given pointer to a menu items user data is not null, this menu
   * item will be disabled.
   *
   * @param p_pUserDataMenuItemToDisable
   * A pointer to an user data field of a menu item which is associated
   * with this window. If the menu item is found by the given user data
   * it will be disabled at window opening time and enabled when the
   * window is closed. Provide NULL if no menu item should be disabled.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  virtual bool Open(APTR p_pUserDataMenuItemToDisable = NULL,
    InitialWindowPosition p_pInitialPosition = IWP_Center) = 0;

  /**
   * Closes the window.
   */
  virtual void Close();

  /**
   * Returns true if the window is opened.
   */
  bool IsOpen();

  const char* Title();
  void SetTitle(SimpleString p_NewTitle);

  /**
   * Returns the intuition window structure or NULL if window is not
   * open.
   */
  struct Window* IntuiWindow();

  /**
   * Returns the app screen object on which the window is opened or
   * NULL if window is not open.
   */
  AppScreen* WindowScreen();



  void SetMenu(AppMenu* p_pMenu);
  void UnsetMenu();

protected:
  AppScreen* m_pAppScreen;
  struct MsgPort* m_pMsgPort;
  struct Window* m_pWindow;

  AppMenu* m_pMenu;
  APTR m_pUserDataMenuItemToDisable;

  SimpleString m_Title;

  virtual void HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress) = 0;

  /**
   * Creates a new window object
   *
   * @param p_pAppScreen
   * Screen on which the window will occur at opening time
   *
   * @param p_pMsgPort
   * Message port which is used for this window. Can be shared
   * with other windows-
   *
   */
  WindowBase(AppScreen* p_pAppScreen, struct MsgPort* p_pMsgPort);

  virtual ~WindowBase();

  /**
   * Creates an BOOPSI image object of one of the system shipped with
   * the operating system from OS2.04 onward.
   *
   * @param p_SysImageId
   * Id of the system image
   *
   * @see intuition/imageclass.h
   *
   * @param p_Width
   * If image creation is successful the width of the image will be
   * stored here.
   *
   * @param p_Height
   * If image creation is successful the height of the image will be
   * stored here.
   *
   * @returns
   * A pointer to the image struct if successful or NULL if failed.
   */
  struct Image* createImageObj(ULONG p_SysImageId, ULONG& p_Width, ULONG& p_Height);

private:
  /**
   * Use this method to close any windows that share an IDCMP port
   * with another window.
   */
  void closeWindowSafely();

  /**
   * Method to remove and reply all IntuiMessages on a port that have
   * been sent to a particular window (note that we don't rely on the
   * ln_Succ pointer of a message after we have replied it)
   */
  void stripIntuiMessages();

};


#endif
