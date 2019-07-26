#include "DiffTracer.h"

DiffTracer::DiffTracer()
{
}

DiffTracer::~DiffTracer()
{
  // Free the memory of all allocated items
  // The list / nodes are freed in the list destructor

  Array<int>* pItem = (Array<int>*) m_TraceList.GetFirst();
  if(pItem == NULL)
  {
    return;
  }

  do
  {
    delete pItem;
  }
  while((pItem = (Array<int>*) m_TraceList.GetNext()) != NULL);

}


void DiffTracer::AddArray(const int* array, size_t arrayLen)
{
  Array<int>* pItem = new Array<int>();

  for(size_t i = 0; i < arrayLen; i++)
  {
    pItem->Push(array[i]);
  }
  
  m_TraceList.InsertTail(pItem);
}


Array<int>* DiffTracer::GetFirst()
{
  return (Array<int>*) m_TraceList.GetFirst();
}


Array<int>* DiffTracer::GetNext()
{
  return (Array<int>*) m_TraceList.GetNext();
}
