#include "DiffFilePartition.h"

DiffFilePartition::DiffFilePartition(LinkedList* p_pLinesList)
  : m_pLinesList(p_pLinesList),
    m_pTokensList(new LinkedList())
{

}

DiffFilePartition::~DiffFilePartition()
{
  clearTokensList();
  delete m_pTokensList;
}

size_t DiffFilePartition::NumberOfLines()
{
  if(m_pLinesList == NULL)
  {
    return 0;
  }

  return m_pLinesList->Size();
}

SimpleString* DiffFilePartition::GetIndexedLine(size_t idx)
{
  SimpleString* pLine = static_cast<SimpleString*>(m_pLinesList->GetIndexed(idx));
  return pLine;
}

LinkedList* DiffFilePartition::TokensList()
{
  return m_pTokensList;
}

bool DiffFilePartition::PreProcess()
{
  SimpleString* pFileLine = static_cast<SimpleString*>(m_pLinesList->GetFirst());
  while(pFileLine != NULL)
  {
    long* pToken = new long[1];
    pToken[0] = calculateLineToken(pFileLine);
    m_pTokensList->InsertTail(pToken);

    pFileLine = static_cast<SimpleString*>(m_pLinesList->GetNext());
  }

  return true;
}

bool DiffFilePartition::MatchLine(size_t i1, DiffFilePartition* p_pOtherFile, size_t i2)
{
  if(m_pTokensList->Size() == 0)
  {
    return false;
  }

  if(i1 > NumberOfLines())
  {
    return false;
  }

  long nf1Token = *static_cast<long*>(m_pTokensList->GetIndexed(i1));

  LinkedList* pF2TokenList = p_pOtherFile->TokensList();
  if(pF2TokenList == NULL)
  {
    return false;
  }

  SimpleString* pLineF1 = GetIndexedLine(i1); // TODO move above while loop?

  bool bFound = false;
  size_t i = 0;
  size_t nf2SubsetLines = p_pOtherFile->NumberOfLines() - i2;

  long nf2Token = *static_cast<long*>(pF2TokenList->GetIndexed(i2));

  while(!bFound && i < nf2SubsetLines)
  {
    if(nf1Token == nf2Token)  // Fast compare
    {
      // Make sure strings really match
      SimpleString* pLineF2 = p_pOtherFile->GetIndexedLine(i2 + i);
      bFound = ((*pLineF1) == (*pLineF2));
    }

    i++;
    if(static_cast<long*>(pF2TokenList->GetNext()) == NULL)
    {
      return false;
    }

    nf2Token = *static_cast<long*>(pF2TokenList->GetSelected());
  }

  i--;

  if(bFound)
  {
    i2 += i;
    return true;
  }

  return false;
}


long DiffFilePartition::calculateLineToken(SimpleString* p_pLine)
{
  long token = 0;
  char* pBuf = p_pLine->C_str();

  for(size_t i = 0; i < p_pLine->Length(); i++)
  {
    token += 2 * token + *(pBuf++); // (George V. Reilly hint)
  }

  return token;
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
