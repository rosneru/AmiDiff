#ifndef APPSCREEN
#define APPSCREEN

#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include "ADiffViewPens.h"
#include "ADiffViewSettings.h"

/**
 * An Amiga screen to be used for an application. This class is
 * abstract, so only a derived class can be instanciated. 
 *
 * Right after instanciaten the screen is open. Destroy the instance to
 * close the screen.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class ScreenBase
{
public:
  ScreenBase(const ADiffViewSettings& settings);
  virtual ~ScreenBase() = 0; // Make it abstract.

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
  struct TextAttr* IntuiTextAttr();

  /**
   * Returns the VisualInfo needed for GadTools menus and Gadgets
   */
  APTR GadtoolsVisualInfo();

  /**
   * Returns the pens to be used for drawing
   */
  const ADiffViewPens& Pens() const;

  size_t NumOpenWindows() const;

  void IncreaseNumOpenWindows();
  void DecreaseNumOpenWindows();

  /**
   * Returns true if the screen is in a Hires mode which means it has
   * non-square pixels.
   */
  bool IsHiresMode() const;

protected:
  struct Screen* m_pIntuiScreen;
  struct DrawInfo* m_pDrawInfo;
  struct TextFont* m_pTextFont;

  ADiffViewPens m_Pens;
  const ADiffViewSettings& m_Settings;

  size_t m_NumWindowsOpen;

  APTR m_pVisualInfo;
};


#endif
