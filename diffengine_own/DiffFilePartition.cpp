#include "DiffFilePartition.h"

DiffFilePartition::DiffFilePartition()
  : m_pInputLinesList(NULL),
    m_pDiffLinesList(new LinkedList),
    m_pTokensList(new LinkedList())
{

}

DiffFilePartition::DiffFilePartition(LinkedList* p_pLinesList)
  : m_pInputLinesList(p_pLinesList),
    m_pDiffLinesList(new LinkedList),
    m_pTokensList(new LinkedList())
{

}

DiffFilePartition::~DiffFilePartition()
{
  clearTokensList();
  delete m_pTokensList;

  clearDiffLinesList();
  delete m_pDiffLinesList;
}

size_t DiffFilePartition::NumberOfLines()
{
  if(m_pDiffLinesList == NULL)
  {
    return 0;
  }

  return m_pDiffLinesList->Size();
}

DiffLine* DiffFilePartition::GetIndexedDiffLine(size_t p_Index)
{
  if(m_pDiffLinesList == NULL)
  {
    return NULL;
  }

  if(p_Index >= m_pDiffLinesList->Size())
  {
    return NULL;
  }

  DiffLine* pDiffLine = static_cast<DiffLine*>(m_pDiffLinesList->GetIndexed(p_Index));
  return pDiffLine;
}

SimpleString* DiffFilePartition::GetIndexedRawLine(size_t p_Index)
{
  if(m_pDiffLinesList == NULL)
  {
    return NULL;
  }

  if(p_Index >= m_pDiffLinesList->Size())
  {
    return NULL;
  }

  DiffLine* pDiffLine = static_cast<DiffLine*>(m_pDiffLinesList->GetIndexed(p_Index));
  return pDiffLine->GetLine();
}

DiffLine::LineState DiffFilePartition::GetIndexedLineState(size_t p_Index)
{
  if(m_pDiffLinesList == NULL)
  {
    return DiffLine::Undefined;
  }

  if(p_Index >= m_pDiffLinesList->Size())
  {
    return DiffLine::Undefined;
  }

  DiffLine* pDiffLine = static_cast<DiffLine*>(m_pDiffLinesList->GetIndexed(p_Index));
  return pDiffLine->GetState();
}

LinkedList* DiffFilePartition::TokensList()
{
  return m_pTokensList;
}

bool DiffFilePartition::PreProcess()
{
  SimpleString* pFileLine = static_cast<SimpleString*>(m_pInputLinesList->GetFirst());
  while(pFileLine != NULL)
  {
    AddString(pFileLine);

    pFileLine = static_cast<SimpleString*>(m_pInputLinesList->GetNext());
  }

  return true;
}

bool DiffFilePartition::MatchLine(long i1, DiffFilePartition& p_OtherFile, long& i2)
{
  if(m_pTokensList->Size() == 0)
  {
    return false;
  }

  if((i1 < 0 ) || (i1 > NumberOfLines()))
  {
    return false;
  }

  long thisFileToken = *static_cast<long*>(m_pTokensList->GetIndexed(i1));

  LinkedList* pOtherFileTokenList = p_OtherFile.TokensList();
  if(pOtherFileTokenList == NULL)
  {
    return false;
  }

  SimpleString* pLineThisFile = GetIndexedDiffLine(i1)->GetLine();

  bool bFound = false;
  long i = 0;
  long otherFileSubsetLines = p_OtherFile.NumberOfLines() - i2;

  long otherFileToken = *static_cast<long*>(pOtherFileTokenList->GetIndexed(i2));

  while(!bFound && i < otherFileSubsetLines)
  {
    if(thisFileToken == otherFileToken)  // Fast compare
    {
      // Make sure strings really match
      SimpleString* pLineOtherFile = p_OtherFile.GetIndexedDiffLine(i2 + i)->GetLine();
      bFound = ((*pLineThisFile) == (*pLineOtherFile));
    }

    i++;
    if(static_cast<long*>(pOtherFileTokenList->GetNext()) == NULL)
    {
      break;
    }

    otherFileToken = *static_cast<long*>(pOtherFileTokenList->GetSelected());
  }

  i--;

  if(bFound)
  {
    i2 += i;
    return true;
  }

  return false;
}

void DiffFilePartition::AddString(SimpleString* p_pString, DiffLine::LineState p_LineState)
{
  DiffLine* pDiffLine = new DiffLine();
  if(pDiffLine != NULL)
  {
    pDiffLine->SetLine(p_pString, p_LineState);
  }

  // Append DiffLine to list
  m_pDiffLinesList->InsertTail(pDiffLine);
}

void DiffFilePartition::AddString(SimpleString* p_pString)
{
  DiffLine* pDiffLine = new DiffLine();
  if(pDiffLine != NULL)
  {
    // Set string in DiffLine gets us the token
    long token = pDiffLine->SetLine(p_pString);

    // Dynamically allocate memory for the token
    long* pToken = new long[1];
    pToken[0] = token;

    // Append token to list
    m_pTokensList->InsertTail(pToken);

    // Append DiffLine to list
    m_pDiffLinesList->InsertTail(pDiffLine);
  }

}

void DiffFilePartition::AddBlankLine()
{
  AddString(NULL, DiffLine::Normal);
}

void DiffFilePartition::clearDiffLinesList()
{
  DiffLine* pDiffLine = static_cast<DiffLine*>(m_pDiffLinesList->GetFirst());
  while(pDiffLine != NULL)
  {
    delete pDiffLine;
    m_pDiffLinesList->RemoveItem();
    pDiffLine = static_cast<DiffLine*>(m_pDiffLinesList->GetFirst());
  }
}


void DiffFilePartition::clearTokensList()
{
  long* pToken = static_cast<long*>(m_pTokensList->GetFirst());
  while(pToken != NULL)
  {
    delete[] pToken;
    m_pTokensList->RemoveItem();
    pToken = static_cast<long*>(m_pTokensList->GetFirst());
  }
}
