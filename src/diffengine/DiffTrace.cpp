#include <stdio.h>

#include "DiffTrace.h"

DiffTrace::DiffTrace()
{
}

DiffTrace::~DiffTrace()
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


void DiffTrace::AddTrace(const int* array, size_t arrayLen)
{
  Array<int>* pItem = new Array<int>();

  for(size_t i = 0; i < arrayLen; i++)
  {
    pItem->Push(array[i]);
  }
  
  m_TraceList.InsertTail(pItem);
}


Array<int>* DiffTrace::GetFirst()
{
  return (Array<int>*) m_TraceList.GetFirst();
}


Array<int>* DiffTrace::GetNext()
{
  return (Array<int>*) m_TraceList.GetNext();
}

size_t DiffTrace::NumItems()
{
  return m_TraceList.Size();
}

void DiffTrace::Backtrack(int x, int y)
{
  int prevK;
  int prevX;
  int prevY;


  Array<int>* v = (Array<int>*) m_TraceList.GetLast();
  for(int d = m_TraceList.Size() - 1; d >= 0; d--)
  {
    if(v == NULL)
    {
      break;
    }

    int k = x - y;

    if((k == -d) || ((k != d) && ((*v)[k - 1] < (*v)[k + 1])))
    {
      prevK = k + 1;
    }
    else
    {
      prevK = k - 1;
    }

    prevX = (*v)[prevK];
    prevY = prevX - prevK;

    while(x > prevX && y > prevY)
    {
      printf("yield #1 x-1, y-1, x, y = %d,%d,%d,%d\n",x-1, y-1, x, y );
      x--;
      y--;
    }

    if(d > 0)
    {
      printf("yield #2 prevX, prevY, x, y = %d,%d,%d,%d\n",prevX, prevY, x, y );
    }


    v = (Array<int>*) m_TraceList.GetPrev();
  }
}

void DiffTrace::Print()
{
  Array<int>* pItem = (Array<int>*)m_TraceList.GetFirst();
  for(int i = 0; i < m_TraceList.Size(); i++)
  {
    if(pItem == NULL)
    {
      return;
    }

    printf("%d: {", i);

    for(int j = 0; j < pItem->Size(); j++)
    {
      printf("%d", (*pItem)[j]);
      if(j < (pItem->Size() - 1))
      {
        printf(", ");
      }
    }

    printf("}\n");

    pItem = (Array<int>*)m_TraceList.GetNext();
  }
}
