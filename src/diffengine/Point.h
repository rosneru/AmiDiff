#ifndef POINT_H
#define POINT_H


/**
 * A class that represents a point.
 *
 * @author Uwe Rosner
 * @date 23/10/2019
 */
class Point
{
public:
  Point(long left, long top);
  Point();
  ~Point();

  long Left() const;
  long Top() const;

  bool IsNil() const;

private:
  bool m_bNil;
  long m_Left;
  long m_Top;
};

#endif
