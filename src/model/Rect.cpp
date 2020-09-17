#include "Rect.h"

Rect::Rect()
  : m_Left(0),
    m_Top(0),
    m_Right(0),
    m_Bottom(0),
    m_WordWidth(0)
{

}

Rect::Rect(unsigned long left, 
           unsigned long top, 
           unsigned long right, 
           unsigned long bottom)
  : m_Left(left),
    m_Top(top),
    m_Right(right),
    m_Bottom(bottom),
    m_WordWidth((((right - left) + 15) & -16) >> 4)
{

}


Rect::Rect(unsigned long left, unsigned long top)
  : m_Left(left),
    m_Top(top),
    m_Right(left),
    m_Bottom(top),
    m_WordWidth(0)
{

}

void Rect::Set(unsigned long left, 
               unsigned long top, 
               unsigned long right, 
               unsigned long bottom)
{
  m_Left = left;
  m_Top = top;
  m_Right = right;
  m_Bottom = bottom;

  m_WordWidth = (((right - left) + 15) & -16) >> 4;
}

void Rect::SetLeftTop(unsigned long left, unsigned long top)
{
  m_Left = left;
  m_Top = top;
}

void Rect::SetWidthHeight(unsigned long width, unsigned long height)
{
  m_Right = m_Left + width - 1;
  m_Bottom = m_Top + height - 1;
  m_WordWidth = ((width + 15) & -16) >> 4;
}

bool Rect::HasSize() const
{
  return (m_Left != m_Right) && (m_Top != m_Bottom);
}


unsigned long Rect::Area() const
{
  return Width() * Height();
}

unsigned long Rect::Left() const
{
  return m_Left;
}


unsigned long Rect::Right() const
{
  return m_Right;
}


unsigned long Rect::Top() const
{
  return m_Top;
}


unsigned long Rect::Bottom() const
{
  return m_Bottom;
}

unsigned long Rect::Height() const
{

  return m_Bottom - m_Top + 1;
}

unsigned long Rect::Width() const
{
  return m_Right - m_Left + 1;
}

unsigned long Rect::WordWidth() const
{
  return m_WordWidth; 
}
