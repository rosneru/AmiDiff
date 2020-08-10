#include "Rect.h"

Rect::Rect()
  : m_Left(0),
    m_Top(0),
    m_Right(0),
    m_Bottom(0),
    m_WordWidth(0)
{

}

Rect::Rect(long left, long top, long right, long bottom)
  : m_Left(left),
    m_Top(top),
    m_Right(right),
    m_Bottom(bottom),
    m_WordWidth((((right - left) + 15) & -16) >> 4)
{

}


Rect::Rect(long left, long top)
  : m_Left(left),
    m_Top(top),
    m_Right(left),
    m_Bottom(top),
    m_WordWidth(0)
{

}

void Rect::Set(long left, long top, long right, long bottom)
{
  m_Left = left;
  m_Top = top;
  m_Right = right;
  m_Bottom = bottom;

  m_WordWidth = (((right - left) + 15) & -16) >> 4;
}

void Rect::SetWidthHeight(long width, long height)
{
  m_Right = m_Left + width;
  m_Bottom = m_Top + height;
  m_WordWidth = ((width + 15) & -16) >> 4;
}

bool Rect::HasSize() const
{
  return (m_Left != m_Right) && (m_Top != m_Bottom);
}


size_t Rect::Area() const
{
  return (m_Right - m_Left) * (m_Bottom - m_Top);
}

long Rect::Left() const
{
  return m_Left;
}


long Rect::Right() const
{
  return m_Right;
}


long Rect::Top() const
{
  return m_Top;
}


long Rect::Bottom() const
{
  return m_Bottom;
}

size_t Rect::Height() const
{
  return m_Bottom - m_Top;
}

size_t Rect::Width() const
{
  return m_Right - m_Left;
}

size_t Rect::WordWidth() const
{
  return m_WordWidth; 
}
