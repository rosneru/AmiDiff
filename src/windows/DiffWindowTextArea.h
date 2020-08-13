#ifndef DIFF_WINDOW_TEXT_AREA_H
#define DIFF_WINDOW_TEXT_AREA_H

#include <exec/types.h>
#include "Rect.h"

class DiffWindowTextArea : public Rect
{
public:
  DiffWindowTextArea(const UWORD& lineNumbersWidth_pix);
  virtual ~DiffWindowTextArea();

  /**
   * Extends base class method.
   *
   * Also calculates the HScroll and VScroll rects wgich depent on the
   * text width of the max possible text width of this tzext area.
   */
  void SetWidthHeightScroll(long width, long height, long maxTextWidth_pix);

  /**
   * Returns the rect of the scrolling area for horizontal scrolling.
   */
  const Rect& HScroll();

  /**
   * Returns the rect of the scrolling area for vertical scrolling.
   */
  const Rect& VScroll();

private:
  const UWORD& m_LineNumbersWidth_pix;
  Rect m_HScrollRect;
  Rect m_VScrollRect;
};

#endif
