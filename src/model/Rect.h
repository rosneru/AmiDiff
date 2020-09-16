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

  /**
   * Sets the rectangle to the given position.
   */
  void Set(long left, long top, long right, long bottom);

  /**
   * Sets the left top edge of the rectangle to the new position.
   *
   * NOTE: This is *not* a move. The right bottom edge is not changed,
   * so the width and height of the rectangle will be different after
   * this call.
   */
  void SetLeftTop(long left, long top);

  /**
   * Sets the width and height of the rectangle to the given sizes.
   *
   * NOTE: The right bottom edge of the rectangle is changed by this
   * operation. The left top edge is not changed.
   */
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
