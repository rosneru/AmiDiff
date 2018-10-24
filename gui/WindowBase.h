#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#include <intuition/screens.h>
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
  virtual ~WindowBase();

  virtual void HandleIdcmp(struct IntuiMessage* p_pMsg) = 0;

  /**
   * Opening the window
   */
  virtual bool Open() = 0;

  /**
   * Closes the window
   */
  virtual void Close();

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
  AppScreen* OnScreen();

protected:
  AppScreen* m_pAppScreen;
  struct Window* m_pWindow;
  SimpleString m_Title;

  /**
   * Creates a new window object
   *
   * @param p_pAppScreen
   * Screen on which the window will occur at opening time
   *
   */
  WindowBase(AppScreen* p_pAppScreen);

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
};


#endif
