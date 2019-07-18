#include "DiffFilePartition.h"

DiffFilePartition::DiffFilePartition(bool& p_bCancelRequested)
  : m_bCancelRequested(p_bCancelRequested),
    m_pProgressReporter(NULL)
{

}

DiffFilePartition::~DiffFilePartition()
{
  Clear();
}

void DiffFilePartition::Clear()
{
  if(m_DiffLinesArray.Size() == 0)
  {
    return;
  }

  DiffLine* pItem;
  while((pItem = m_DiffLinesArray.Pop()) != NULL)
  {
    delete pItem;

    if(m_DiffLinesArray.Size() == 0)
    {
      break;
    }
  }
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

    DiffLine::LineState lineState = GetDiffLineState(i);
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

const DiffLine* DiffFilePartition::GetDiffLine(size_t p_Index) const
{
  if(p_Index >= m_DiffLinesArray.Size())
  {
    return NULL;
  }

  return m_DiffLinesArray[p_Index];
}

static const SimpleString emptyStr = "";

const SimpleString& DiffFilePartition::GetDiffLineText(size_t p_Index) const
{
  if(m_DiffLinesArray.IsEmpty()|| p_Index >= m_DiffLinesArray.Size())
  {
    return emptyStr;
  }

  return GetDiffLine(p_Index)->Text();
}

DiffLine::LineState DiffFilePartition::GetDiffLineState(size_t p_Index) const
{
  if(m_DiffLinesArray.IsEmpty() || p_Index >= m_DiffLinesArray.Size())
  {
    return DiffLine::Undefined;
  }

  return GetDiffLine(p_Index)->State();
}


bool DiffFilePartition::PreProcess()
{
  int i = 0;
  SimpleString* pSrcLine = m_InputLinesArray[i];
  while(pSrcLine != NULL)
  {
    AddString(*pSrcLine);

    pSrcLine = m_InputLinesArray[i];
    i++;
  }

  return true;
}


void DiffFilePartition::AddString(const SimpleString& p_String, 
                                  DiffLine::LineState p_LineState)
{
  DiffLine* pDiffLine = new DiffLine();
  if(pDiffLine == NULL)
  {
    return;
  }

  pDiffLine->SetLine(p_String, p_LineState);
  m_DiffLinesArray.Push(pDiffLine);
}

void DiffFilePartition::AddString(const SimpleString& p_String)
{
  DiffLine* pDiffLine = new DiffLine();
  if(pDiffLine == NULL)
  {
    return;
  }

  // Set string in DiffLine gets us the token
  pDiffLine->SetLine(p_String);

  // Append DiffLine to list
  m_DiffLinesArray.Push(pDiffLine);
}

void DiffFilePartition::AddBlankLine()
{
  AddString(SimpleString(), DiffLine::Normal);
}

void DiffFilePartition::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}
