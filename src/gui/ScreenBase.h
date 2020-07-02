#ifndef APPSCREEN
#define APPSCREEN

#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include "ADiffViewPens.h"
#include "ADiffViewSettings.h"

/**
 * Class  for the application main screen. At the moment it simply makes
 * an identical copy of the Workbench screen.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class ScreenBase
{
public:
  ScreenBase(const ADiffViewSettings& settings);
  virtual ~ScreenBase();

  /**
   * Open the screen
   *
   * @returns
   * false if oping fails
   */
  virtual bool Open() = 0;

  /**
   * Close the screen
   */
  virtual void Close() = 0;

  bool IsOpen() const;

  UWORD FontHeight() const;
  WORD BarHeight() const;


  /**
   * Returns the intuition screen structure or NULL if screen is not open
   */
  struct Screen* IntuiScreen();

  /**
   * Returns the pointer to the active window. If no active window is
   * found NULL is returned.
   *
   * NOTE: I) The active window is not guaranteed to be on this screen.
   *
   *       II) Be aware that the active window can already be another
   *       one directly after this call as the user or the application
   *       can change it.
   *
   *       III.) The active window can already have been closed when
   *       you use this window pointer. Be prepared!
   */
  struct Window* ActiveWindow() const;

  /**
   * Returns the intuition screen draw info or NULL if it hadn't been
   * acquired successfully
   */
  struct DrawInfo* IntuiDrawInfo();

  /**
   * Returns the TextAttr of the screen
   */
  struct TextAttr* IntuiTextAttr();

  /**
   * Returns the VisualInfo needed for GadTools menus and Gadgets
   */
  APTR GadtoolsVisualInfo();

  /**
   * Returns the pens to be used for drawing
   */
  const ADiffViewPens& Pens() const;


protected:
  struct Screen* m_pScreen;
  struct DrawInfo* m_pDrawInfo;
  struct TextFont* m_pTextFont;

  ADiffViewPens m_Pens;
  const ADiffViewSettings& m_Settings;

  APTR m_pVisualInfo;
};


#endif
