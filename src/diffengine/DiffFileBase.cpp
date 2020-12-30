#include "DiffFileBase.h"

DiffFileBase::DiffFileBase()
  : m_EmptyText('\0')
{

}


size_t DiffFileBase::NumLines() const
{
  return m_Lines.size();
}


DiffLine* DiffFileBase::operator[](unsigned long index) const
{
  if(index >= m_Lines.size())
  {
    return NULL;
  }

  return m_Lines[index];
}


void DiffFileBase::SetLineState(size_t idx, DiffLine::LineState state)
{
  DiffLine* pDiffLine = (*this)[idx];
  if(pDiffLine == NULL)
  {
    // TODO Maybe change method to type bool and return false
    return;
  }

  pDiffLine->SetState(state);
}

size_t DiffFileBase::numDigits(size_t number)
{
  size_t digits = 1;
  if ( number >= 100000000 ) { digits += 8; number /= 100000000; }
  if ( number >= 10000     ) { digits += 4; number /= 10000; }
  if ( number >= 100       ) { digits += 2; number /= 100; }
  if ( number >= 10        ) { digits += 1; }

  return digits;
}