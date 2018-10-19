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
   * Closes the window
   */
  virtual void Close();

  const char* Title();
  void SetTitle(SimpleString p_NewTitle);

  /**
   * Returns the intuition window structure or NULL if window is not open
   */
  struct Window* IntuiWindow();

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
};


#endif
