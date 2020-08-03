#include "DiffFileBase.h"

DiffFileBase::DiffFileBase()
  : m_NumLines(0),
    m_EmptyText('\0')
{

}


size_t DiffFileBase::NumLines() const
{
  return m_NumLines;
}


DiffLine* DiffFileBase::GetLine(size_t idx) const
{
  if(idx >= m_NumLines)
  {
    return NULL;
  }

  return m_Lines[idx];
}


const char* DiffFileBase::GetLineText(size_t idx) const
{
  if(idx >= m_NumLines)
  {
    return &m_EmptyText;
  }

  return GetLine(idx)->Text();
}


const char* DiffFileBase::GetLineNum(size_t idx) const
{
  if(idx >= m_NumLines)
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
  if(idx >= m_NumLines)
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


void DiffFileBase::setNumLines(size_t numLines)
{
  m_NumLines = numLines;
}


void DiffFileBase::decrementNumLines()
{
  m_NumLines--;
}
