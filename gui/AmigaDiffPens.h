#ifndef AMIGA_DIFF_PENS_H
#define AMIGA_DIFF_PENS_H

#include <intuition/screens.h>

class AppScreen;

/**
 * Class for the pens used by AmigaDiff
 *
 * @author Uwe Rosner
 * @date 18/11/2018
 */
class AmigaDiffPens
{
public:
  AmigaDiffPens();
  ~AmigaDiffPens();

  /**
   * Creates the colors needed by AmigaDiff.
   *
   * Starting with color number p_FirstFreeColorNum the needed 4 colors
   * will be assigned to color registers.
   */
  bool Init(AppScreen* p_pAppScreen);

  /**
   * Freeing pens, allocated colormaps etc
   */
  void Dispose();

  LONG Background() const;
  LONG Text() const;
  LONG HighlightedText() const;

  LONG Red() const;
  LONG Yellow() const;
  LONG Green() const;
  LONG Grey() const;

private:
  AppScreen* m_pAppScreen;
  bool m_bInitialized;
  bool m_bObtainedPens;
  LONG m_RedPen;
  LONG m_YellowPen;
  LONG m_GreenPen;
  LONG m_GreyPen;
};

#endif
