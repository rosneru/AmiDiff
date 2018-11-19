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
  enum ObtainScreenMode
  {
    OSM_UseWb,      ///> Uses to Worbkbench public screen
    OSM_CloneWb,    ///> Clones the workbench screen
    OSM_CloneWb3Bp, ///> Clones the Workbench screen an ensures at least 3 bitplanes (8 colors)
  };

  AppScreen(ObtainScreenMode p_ObtainedScreenMode);
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


private:
  ObtainScreenMode m_ObtainedScreenMode;
  AmigaDiffPens m_Pens;
  struct TextAttr m_TextAttr;
  struct TextFont* m_pTextFont;
  SimpleString m_FontName;
  SimpleString m_Title;
  struct Screen* m_pScreen;
  struct DrawInfo* m_pDrawInfo;
  APTR* m_pVisualInfo;

};


#endif
