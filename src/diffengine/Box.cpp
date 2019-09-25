#include "Box.h"

Box::Box(long left, long top, long right, long bottom)
  : m_Left(left),
    m_Top(top),
    m_Right(right),
    m_Bottom(bottom)
{
}

Box::~Box()
{

}

long Box::Width() const
{
  return m_Right - m_Left;
}

long Box::Height() const
{
  return m_Bottom - m_Top;
}

long Box::Size() const
{
  return Width() + Height();
}

long Box::Delta() const
{
  return Width() - Height();
}
