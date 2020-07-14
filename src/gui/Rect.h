#ifndef RECT_H
#define RECT_H

#include <stddef.h>

/**
 * Represents a simple, geometric rectangle.
 *
 * @author Uwe Rosner
 * @date 10/06/2020
 */
class Rect
{
public:
  Rect();
  Rect(long left, long top, long right, long bottom);
  Rect(long left, long top);

  void Set(long left, long top, long right, long bottom);
  void SetWidthHeight(long width, long height);

  bool HasSize() const;
  size_t Area() const;

  long Left() const;
  long Right() const;
  long Top() const;
  long Bottom() const;
  
  size_t Height() const;
  size_t Width() const;
  size_t WordWidth() const;

private:
  long m_Left;
  long m_Top;
  long m_Right;
  long m_Bottom;

  long m_WordWidth;
};

#endif
