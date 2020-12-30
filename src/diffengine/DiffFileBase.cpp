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



const char* DiffFileBase::GetLineNum(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return &m_EmptyText;
  }

  const char* pLineNum = (*this)[idx]->LineNum();
  if(pLineNum == NULL)
  {
    return &m_EmptyText;
  }

  return pLineNum;
}


unsigned long DiffFileBase::GetLineToken(size_t idx) const
{
  DiffLine* pLine = (*this)[idx];
  if(pLine == NULL)
  {
    return 0;
  }

  return (*this)[idx]->Token();
}


DiffLine::LineState DiffFileBase::GetLineState(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return DiffLine::Undefined;
  }

  return (*this)[idx]->State();
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