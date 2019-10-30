#ifndef PAIR_H
#define PAIR_H


/**
 * A class that represents a Pair.
 *
 * @author Uwe Rosner
 * @date 23/10/2019
 */
class Pair
{
public:
  Pair(long left, long top);
  Pair();
  ~Pair();

  long Left() const;
  long Top() const;

  bool IsNil() const;

private:
  bool m_bNil;
  long m_Left;
  long m_Top;
};

#endif
