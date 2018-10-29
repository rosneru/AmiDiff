#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

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
  virtual ~WindowBase();

  virtual void HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress) = 0;

  /**
   * Opening the window
   */
  virtual bool Open() = 0;

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
  struct Window* m_pWindow;
  AppMenu* m_pMenu;
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
