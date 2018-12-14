#include "DiffFilePartition.h"

DiffFilePartition::DiffFilePartition()
  : m_Index(0)
{

}

DiffFilePartition::~DiffFilePartition()
{
  clearDiffLinesList();
}

long DiffFilePartition::NumLines() const
{
  return m_DiffLinesArray.Size();
}

void DiffFilePartition::NumChanges(int& p_Added, int& p_Changed, int& p_Deleted)
{
  p_Added = p_Changed = p_Deleted = 0;
  if(NumLines() == 0)
  {
    return;
  }

  DiffLine* pDiffLine = GetFirstDiffLine();
  long i = 0;
  while((i < NumLines()) && (pDiffLine != NULL))
  {
    DiffLine::LineState lineState = pDiffLine->GetState();
    switch(lineState)
    {
      case DiffLine::Added: p_Added++; break;
      case DiffLine::Changed: p_Changed++; break;
      case DiffLine::Deleted: p_Deleted++; break;

      case DiffLine::Normal:
      case DiffLine::Undefined:
       break;
    }

    pDiffLine = GetNextDiffLine();
    i++;
  }
}


DiffLine* DiffFilePartition::GetIndexedDiffLine(size_t p_Index)
{
  if(p_Index >= m_DiffLinesArray.Size())
  {
    return NULL;
  }

  m_Index = p_Index;
  return m_DiffLinesArray[p_Index];
}

DiffLine* DiffFilePartition::GetFirstDiffLine()
{
  if(m_DiffLinesArray.IsEmpty())
  {
    return NULL;
  }

  m_Index = 0;
  return m_DiffLinesArray[m_Index];
}

DiffLine* DiffFilePartition::GetNextDiffLine()
{
  if(m_DiffLinesArray.IsEmpty() || m_Index >= m_DiffLinesArray.Size())
  {
    return NULL;
  }

  m_Index++;
  return m_DiffLinesArray[m_Index];
}

DiffLine* DiffFilePartition::GetPreviousDiffLine()
{
  if(m_DiffLinesArray.IsEmpty() || m_Index < 1)
  {
    return NULL;
  }

  m_Index--;

  return m_DiffLinesArray[m_Index];
}

DiffLine* DiffFilePartition::GetCurrentDiffLine()
{
  if(m_DiffLinesArray.IsEmpty())
  {
    return NULL;
  }

  return m_DiffLinesArray[m_Index];
}

const SimpleString DiffFilePartition::GetIndexedLineText(size_t p_Index)
{
  if(m_DiffLinesArray.IsEmpty()|| p_Index >= m_DiffLinesArray.Size())
  {
    return SimpleString();
  }

  return GetIndexedDiffLine(p_Index)->GetText();
}

DiffLine::LineState DiffFilePartition::GetIndexedLineState(size_t p_Index)
{
  if(m_DiffLinesArray.IsEmpty() || p_Index >= m_DiffLinesArray.Size())
  {
    return DiffLine::Undefined;
  }

  return GetIndexedDiffLine(p_Index)->GetState();
}

Array<long>& DiffFilePartition::TokensList()
{
  return m_TokensArray;
}

bool DiffFilePartition::PreProcess()
{
  int i = 0;
  SimpleString* pFileLine = m_InputLinesArray[i];
  while(pFileLine != NULL)
  {
    AddString(*pFileLine);

    pFileLine = m_InputLinesArray[i];
    i++;
  }

  return true;
}

bool DiffFilePartition::MatchLine(long i1, DiffFilePartition& p_OtherFile, long& i2)
{
  if(m_TokensArray.Size() == 0)
  {
    return false;
  }

  if((i1 < 0 ) || (i1 > NumLines()))
  {
    return false;
  }

  long* otherFileTokenArray = p_OtherFile.TokensList().Data();
  if(otherFileTokenArray == NULL)
  {
    return false;
  }

  const SimpleString lineThisFile = GetIndexedDiffLine(i1)->GetText();

  bool bFound = false;
  long i = 0;
  long otherFileSubsetLines = p_OtherFile.NumLines() - i2;

  long* pOtherFileToken = otherFileTokenArray + i2;

  while(!bFound && i < otherFileSubsetLines)
  {
    if(m_TokensArray[i1] == *pOtherFileToken)  // Fast compare
    {
      // Make sure strings really match
      const SimpleString lineOtherFile = p_OtherFile.GetIndexedDiffLine(i2 + i)->GetText();
      bFound = (lineThisFile == lineOtherFile);
    }

    pOtherFileToken++;
    i++;
    if(pOtherFileToken == NULL)
    {
      break;
    }
  }

  i--;

  if(bFound)
  {
    i2 += i;
    return true;
  }

  return false;
}

void DiffFilePartition::AddString(const SimpleString& p_String, DiffLine::LineState p_LineState)
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
  long token = pDiffLine->SetLine(p_String);

  // Append token to list
  m_TokensArray.Push(token);

  // Append DiffLine to list
  m_DiffLinesArray.Push(pDiffLine);

}

void DiffFilePartition::AddBlankLine()
{
  AddString(SimpleString(), DiffLine::Normal);
}

void DiffFilePartition::clearDiffLinesList()
{
  for(size_t i=0; i < m_DiffLinesArray.Size(); i++)
  {
    DiffLine* pDiffLine = m_DiffLinesArray[i];
    if(pDiffLine != NULL)
    {
      delete pDiffLine;
    }
  }
}

