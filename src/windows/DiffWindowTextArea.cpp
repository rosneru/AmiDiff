#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(const UWORD& lineNumbersWidth)
  : m_LineNumbersWidth(lineNumbersWidth)
{

}

DiffWindowTextArea::~DiffWindowTextArea()
{

}

void DiffWindowTextArea::SetWidthHeight(long width, long height)
{
  Rect::SetWidthHeight(width, height);

  //
  // Define the dimensions for the scroll areas
  // NOTE: The 'Left' values depend on if the scroll is done 
  //       horizontally or vertically. On vertical scroll the line 
  //       numbers are scrolled too. On horizontal scroll, they're not.
  //
  m_HScrollRect.Set(Left() + m_LineNumbersWidth + 3,
                    Top() + 1,
                    Left() + Width() + 2,
                    Top() + Height() - 3);

  m_VScrollRect.Set(Left() + 3,
                    Top() + 1,
                    Left() + Width() + 2,
                    Top() + Height() - 3);
}


const Rect& DiffWindowTextArea::HScroll()
{
  return m_HScrollRect;
}

const Rect& DiffWindowTextArea::VScroll()
{
  return m_VScrollRect;
}
