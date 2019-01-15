#ifndef APPSCREEN
#define APPSCREEN

#include <graphics/text.h>
#include <intuition/screens.h>

#include "AmigaDiffPens.h"
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
  /**
   * Used to specify the needed screen mode without messing around with
   * too many parameters.
   */
  enum ScreenModeEasy
  {
    SME_UseWorkbench,           ///> Uses to Worbkbench public screen
    SME_CloneWorkbench,         ///> Clones the Workbench screen
    SME_CloneWorkbenchMin8Col,  ///> Creates a Workbench screen clone with at 8 colors
    SME_UseNamedPubScreen       ///> Use a pub screen which name is to be given in constructor
  };

  AppScreen();
  ~AppScreen();

  /**
   * Opens a Workbench clone screen
   *
   * @returns
   * false if oping fails
   */
  bool Open(ScreenModeEasy p_ScreenModeEasy = SME_CloneWorkbenchMin8Col,
    SimpleString p_PubScreenName = "");

  /**
   * Closes the screen
   */
  void Close();

  bool IsOpen() const;

  const char* Title() const;
  void SetTitle(SimpleString p_NewTitle); ///> Not working when the screen is already open

  WORD FontHeight() const;

  WORD BarHeight() const;


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

  /**
   * Returns the pens to be used for drawing
   */
  const AmigaDiffPens& Pens() const;

  ScreenModeEasy ScreenMode() const;


private:
  SimpleString m_PubScreenName;
  SimpleString m_FontName;
  SimpleString m_Title;
  ScreenModeEasy m_ScreenModeEasy;
  struct TextAttr m_TextAttr;

  struct Screen* m_pScreen;
  struct DrawInfo* m_pDrawInfo;
  struct TextFont* m_pTextFont;
  AmigaDiffPens m_Pens;
  APTR* m_pVisualInfo;
};


#endif
