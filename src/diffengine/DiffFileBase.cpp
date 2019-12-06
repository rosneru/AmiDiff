#include "DiffFileBase.h"

DiffFileBase::DiffFileBase(bool& bCancelRequested)
  : m_bCancelRequested(bCancelRequested),
    m_pProgressReporter(NULL),
    m_NumLines(0),
    m_pDiffLinesArray(NULL),
    m_NextAddedLineIdx(0)
{

}


long DiffFileBase::NumLines() const
{
  return m_NumLines;
}

void DiffFileBase::SetNumLines(long numLines)
{
  m_NumLines = numLines;
  m_NextAddedLineIdx = 0;
}

void DiffFileBase::DecrementNumLines()
{
  m_NumLines--;
}


DiffLine* DiffFileBase::GetLine(size_t idx) const
{
  if(idx >= m_NumLines)
  {
    return NULL;
  }

  return m_pDiffLinesArray[idx];
}


static const char* pEmptyText = "";

const char* DiffFileBase::GetLineText(size_t idx) const
{
  if(idx >= m_NumLines)
  {
    return pEmptyText;
  }

  return GetLine(idx)->Text();
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

void DiffFileBase::AddBlankLine()
{
  static const char* pEmptyLine = "";
  AddString(pEmptyLine, DiffLine::Normal);
}

void DiffFileBase::SetProgressReporter(ProgressReporter* pProgressReporter)
{
  m_pProgressReporter = pProgressReporter;
}
