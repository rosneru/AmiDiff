#ifndef BOX_H
#define BOX_H


/**
 * A class that represents a box.
 *
 * @author Uwe Rosner
 * @date 25/09/2019
 */
class Box
{
public:
  Box(long left, long top, long right, long bottom);
  ~Box();

  long Width() const;
  long Height() const ;
  long Size() const;
  long Delta() const;

private:
  long m_Left;
  long m_Top;
  long m_Right;
  long m_Bottom;
};

#endif
