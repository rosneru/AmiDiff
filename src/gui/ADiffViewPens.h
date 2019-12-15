#ifndef A_DIFF_VIEW_PENS_H
#define A_DIFF_VIEW_PENS_H

#include <graphics/view.h>
#include <intuition/screens.h>

#include "ADiffViewSettings.h"

/**
 * Class for the pens used by AmigaDiff
 *
 * @author Uwe Rosner
 * @date 18/11/2018
 */
class ADiffViewPens
{
public:
  ADiffViewPens(struct Screen*& pScreen,
                struct DrawInfo*& pDrawInfo,
                ADiffViewSettings& settings);

  ~ADiffViewPens();

  /**
   * Create the color pens needed by AmigaDiff.
   *
   */
  bool Create();

  /**
   * Free allocated pens
   */
  void Dispose();


  LONG Background() const;
  LONG Text() const;
  LONG HighlightedText() const;
  LONG Fill() const;

  LONG Red() const;
  LONG Yellow() const;
  LONG Green() const;


private:
  struct Screen*& m_pScreen;
  struct DrawInfo*& m_pDrawInfo;
  ADiffViewSettings& m_Settings;

  LONG m_RedPen;
  LONG m_YellowPen;
  LONG m_GreenPen;

};

#endif

