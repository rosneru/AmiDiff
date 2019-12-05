#include "DiffFileBase.h"

DiffFileBase::DiffFileBase(bool& p_bCancelRequested)
  : m_bCancelRequested(p_bCancelRequested),
    m_pProgressReporter(NULL),
    m_NumLines(0),
    m_pDiffLines(NULL),
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

void DiffFileBase::NumChanges(int& p_Added,
                                   int& p_Changed,
                                   int& p_Deleted) const
{
  p_Added = p_Changed = p_Deleted = 0;
  if(NumLines() == 0)
  {
    return;
  }

  for(int i = 0; i < NumLines(); i++)
  {

    DiffLine::LineState lineState = GetLineState(i);
    switch(lineState)
    {
      case DiffLine::Added: p_Added++; break;
      case DiffLine::Changed: p_Changed++; break;
      case DiffLine::Deleted: p_Deleted++; break;

      case DiffLine::Normal:
      case DiffLine::Undefined:
       break;
    }
  }
}

DiffLine* DiffFileBase::GetLine(size_t p_Index) const
{
  if(p_Index >= m_NumLines)
  {
    return NULL;
  }

  return m_pDiffLines[p_Index];
}


static const char* pEmptyText = "";

const char* DiffFileBase::GetLineText(size_t p_Index) const
{
  if(p_Index >= m_NumLines)
  {
    return pEmptyText;
  }

  return GetLine(p_Index)->Text();
}

unsigned long DiffFileBase::GetLineToken(size_t p_Index) const
{
  DiffLine* pLine = GetLine(p_Index);
  if(pLine == NULL)
  {
    return 0;
  }

  return GetLine(p_Index)->Token();
}

DiffLine::LineState DiffFileBase::GetLineState(size_t p_Index) const
{
  if(p_Index >= m_NumLines)
  {
    return DiffLine::Undefined;
  }

  return GetLine(p_Index)->State();
}

void DiffFileBase::SetLineState(size_t p_Index, DiffLine::LineState state)
{
  DiffLine* pDiffLine = GetLine(p_Index);
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

void DiffFileBase::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}
