#ifndef A_DIFF_VIEW_PENS_H
#define A_DIFF_VIEW_PENS_H

#include <graphics/view.h>
#include <intuition/screens.h>


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
                struct DrawInfo*& pDrawInfo);
  ~ADiffViewPens();

  /**
   * Creates the colors needed by AmigaDiff.
   *
   * Starting with color number p_FirstFreeColorNum the needed 4 colors
   * will be assigned to color registers.
   */
  bool Create();

  /**
   * Freeing pens, allocated colormaps etc
   */
  void Dispose();


  LONG Background() const;
  LONG Text() const;
  LONG HighlightedText() const;
  LONG Fill() const;

  LONG Red() const;
  LONG Yellow() const;
  LONG Green() const;
  LONG Gray() const;

private:
  struct Screen*& m_pScreen;
  struct DrawInfo*& m_pDrawInfo;

  struct ColorMap* m_pColorMap;

  LONG m_RedPen;
  LONG m_YellowPen;
  LONG m_GreenPen;
  LONG m_GrayPen;

};

#endif

