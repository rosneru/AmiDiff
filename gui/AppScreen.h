#ifndef APPSCREEN
#define APPSCREEN

#include <graphics/text.h>
#include <intuition/screens.h>

#include "SimpleString.h"

/**
 * Class  for the application main screen. At the moment it simply makes
 * an identical copy of the Workbench screen.
 *
 * @author Uwe Rosner
 * @date 23/09/2018   
 */
class AppScreen
{
public:
  AppScreen(SimpleString p_Title);
  ~AppScreen();

  /**
   * Opens a Workbench clone screen
   *
   * @returns
   * false if oping fails
   */
  bool Open();

  /**
   * Closes the screen
   */
  void Close();

  const char* Title();

  /**
   * Returns the intuition screen structure or NULL if screen is not open
   */
  struct Screen* IntuiScreen();

  /**
   * Returns the intuition screen draw info or NULL if it hadn't been
   * acquired successfully
   */
  struct DrawInfo* IntuiDrawInfo();

  /**
   * Returns the TextAttr of the screen
   */
  struct TextAttr* GfxTextAttr();

  /**
   * Returns the VisualInfo needed for GadTools menus and Gadgets
   */
  APTR* GadtoolsVisualInfo();

private:
  struct TextAttr m_TextAttr;
  struct TextFont* m_pTextFont;
  SimpleString m_FontName;
  SimpleString m_Title;
  struct Screen* m_pScreen;
  struct DrawInfo* m_pDrawInfo;
  APTR* m_pVisualInfo;

};


#endif
