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
