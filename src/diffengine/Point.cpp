#include "Point.h"

Point::Point(long left, long top)
  : m_Left(left),
    m_Top(top),
    m_bNil(false)
{
}

Point::Point()
  : m_Left(0),
    m_Top(0),
    m_bNil(true)
{
}

Point::~Point()
{

}

long Point::Left() const
{
  return m_Left;
}

long Point::Top() const
{
  return m_Top;
}

bool Point::IsNil() const
{
  return m_bNil;
}
