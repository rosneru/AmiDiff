#include "DiffFileBase.h"

DiffFileBase::DiffFileBase()
  : m_EmptyText('\0')
{

}


size_t DiffFileBase::NumLines() const
{
  return m_Lines.size();
}


DiffLine* DiffFileBase::GetLine(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return NULL;
  }

  return m_Lines[idx];
}


const char* DiffFileBase::GetLineText(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return &m_EmptyText;
  }

  return GetLine(idx)->Txt();
}


const char* DiffFileBase::GetLineNum(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return &m_EmptyText;
  }

  const char* pLineNum = GetLine(idx)->LineNum();
  if(pLineNum == NULL)
  {
    return &m_EmptyText;
  }

  return pLineNum;
}


unsigned long DiffFileBase::GetLineToken(size_t idx) const
{
  DiffLine* pLine = GetLine(idx);
  if(pLine == NULL)
  {
    return 0;
  }

  return GetLine(idx)->Token();
}


DiffLine::LineState DiffFileBase::GetLineState(size_t idx) const
{
  if(idx >= m_Lines.size())
  {
    return DiffLine::Undefined;
  }

  return GetLine(idx)->State();
}


void DiffFileBase::SetLineState(size_t idx, DiffLine::LineState state)
{
  DiffLine* pDiffLine = GetLine(idx);
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