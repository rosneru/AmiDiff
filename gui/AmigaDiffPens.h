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
  bool Init(AppScreen* p_pAppScreen,short p_FirstFreeColorNum);

  ULONG Background() const;
  ULONG Text() const;
  ULONG HighlightedText() const;

private:
  AppScreen* m_pAppScreen;
  short m_FirstColorNum;
};

#endif
