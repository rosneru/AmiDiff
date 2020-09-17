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
  Rect(unsigned long left, 
       unsigned long top, 
       unsigned long right, 
       unsigned long bottom);

  Rect(unsigned long left, unsigned long top);

  /**
   * Sets the rectangle to the given position.
   */
  void Set(unsigned long left, 
           unsigned long top, 
           unsigned long right, 
           unsigned long bottom);

  /**
   * Sets the left top edge of the rectangle to the new position.
   *
   * NOTE: This is *not* a move. The right bottom edge is not changed,
   * so the width and height of the rectangle will be different after
   * this call.
   */
  void SetLeftTop(unsigned long left, unsigned long top);

  /**
   * Sets the width and height of the rectangle to the given sizes.
   *
   * NOTE: The right bottom edge of the rectangle is changed by this
   * operation. The left top edge is not changed.
   */
  void SetWidthHeight(unsigned long width, unsigned long height);

  bool HasSize() const;
  unsigned long Area() const;

  unsigned long Left() const;
  unsigned long Right() const;
  unsigned long Top() const;
  unsigned long Bottom() const;
  
  unsigned long Height() const;
  unsigned long Width() const;
  unsigned long WordWidth() const;

private:
  unsigned long m_Left;
  unsigned long m_Top;
  unsigned long m_Right;
  unsigned long m_Bottom;

  unsigned long m_WordWidth;
};

#endif
