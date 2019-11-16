#include "DiffFilePartition.h"

DiffFilePartition::DiffFilePartition(bool& p_bCancelRequested)
  : m_bCancelRequested(p_bCancelRequested),
    m_pProgressReporter(NULL)
{

}


long DiffFilePartition::NumLines() const
{
  return m_DiffLinesArray.Size();
}

void DiffFilePartition::NumChanges(int& p_Added,
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

DiffLine* DiffFilePartition::GetLine(size_t p_Index) const
{
  size_t numLines = m_DiffLinesArray.Size();
  if((numLines == 0) || (p_Index >= numLines))
  {
    return NULL;
  }

  return m_DiffLinesArray[p_Index];
}


static const char* pEmptyText = "";

const char* DiffFilePartition::GetLineText(size_t p_Index) const
{
  size_t numLines = m_DiffLinesArray.Size();
  if((numLines == 0) || (p_Index >= numLines))
  {
    return pEmptyText;
  }

  return GetLine(p_Index)->Text();
}

unsigned long DiffFilePartition::GetLineToken(size_t p_Index) const
{
  DiffLine* pLine = GetLine(p_Index);
  if(pLine == NULL)
  {
    return 0;
  }

  return GetLine(p_Index)->Token();
}

DiffLine::LineState DiffFilePartition::GetLineState(size_t p_Index) const
{
  size_t numLines = m_DiffLinesArray.Size();
  if((numLines == 0) || (p_Index >= numLines))
  {
    return DiffLine::Undefined;
  }

  return GetLine(p_Index)->State();
}

void DiffFilePartition::SetLineState(size_t p_Index, DiffLine::LineState state)
{
  DiffLine* pDiffLine = GetLine(p_Index);
  if(pDiffLine == NULL)
  {
    // TODO Maybe change method to type bool and return false
    return;
  }

  pDiffLine->SetState(state);
}

void DiffFilePartition::AddBlankLine()
{
  static const char* pEmptyLine = "";
  AddString(pEmptyLine, DiffLine::Normal);
}

void DiffFilePartition::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}
