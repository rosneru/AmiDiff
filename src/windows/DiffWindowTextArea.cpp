#include <clib/graphics_protos.h>

#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(DiffWindowRastports*& pRPorts, 
                                       const UWORD& lineNumbersWidth_pix)
  : m_pRPorts(pRPorts),
    m_LineNumbersWidth_pix(lineNumbersWidth_pix)
{

}

DiffWindowTextArea::~DiffWindowTextArea()
{

}

void DiffWindowTextArea::Clear()
{
  EraseRect(m_pRPorts->APenBackgr(),
            Left() + 2, 
            Top() + 2,
            Right() - 3,
            Bottom() - 3);
}

void DiffWindowTextArea::SetWidthHeightScroll(long width, 
                                              long height, 
                                              long maxTextWidth_pix)
{
  Rect::SetWidthHeight(width, height);

  //
  // Define the dimensions for the scroll areas
  // NOTE: The 'Left' values depend on if the scroll is done 
  //       horizontally or vertically. On vertical scroll the line 
  //       numbers are scrolled too. On horizontal scroll, they're not.
  //
  m_HScrollRect.Set(Left() + m_LineNumbersWidth_pix + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);

  m_VScrollRect.Set(Left() + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
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
