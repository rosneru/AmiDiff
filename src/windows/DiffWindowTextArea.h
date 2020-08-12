#ifndef DIFF_WINDOW_TEXT_AREA_H
#define DIFF_WINDOW_TEXT_AREA_H

#include <exec/types.h>
#include "Rect.h"

class DiffWindowTextArea : public Rect
{
public:
  DiffWindowTextArea(const UWORD& lineNumbersWidth);
  virtual ~DiffWindowTextArea();

  /**
   * Extends base class method.
   * 
   * Also calculates the HScroll and VScroll rects.
   */
  void SetWidthHeight(long width, long height);

  /**
   * Returns the rect of the scrolling area for horizontal scrolling.
   */
  const Rect& HScroll();

  /**
   * Returns the rect of the scrolling area for vertical scrolling.
   */
  const Rect& VScroll();

private:
  const UWORD& m_LineNumbersWidth;
  Rect m_HScrollRect;
  Rect m_VScrollRect;
};

#endif
