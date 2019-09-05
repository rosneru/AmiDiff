#include <stdio.h>

#include "DiffTrace.h"

DiffTrace::DiffTrace(DiffFilePartition& a, DiffFilePartition& b)
  : m_A(a),
    m_B(b)
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

int idxConv(int i, int from, int to)
{
  if(i < 0) // from instead 0 ??
  {
    return from + to + i;
  }

  if(i > to)
  {
    return from + (i - to);
  }

  return i;
}

void DiffTrace::Backtrack(int x, int y)
{
  int prevK;
  int prevX;
  int prevY;


  int vSize = m_TraceList.Size();
  Array<int>* v = (Array<int>*) m_TraceList.GetLast();
  for(int d = m_TraceList.Size() - 1; d >= 0; d--)
  {
    if(v == NULL)
    {
      break;
    }

    int k = x - y;

    if((k == -d) || ((k != d) && ((*v)[idxConv(k - 1, 1, vSize)] < (*v)[idxConv(k + 1, 1, vSize)])))
    {
      prevK = k + 1;
    }
    else
    {
      prevK = k - 1;
    }

    int idx = idxConv(prevK, 1, vSize);
    prevX = (*v)[idx];
    prevY = prevX - prevK;

    while((x > prevX) && (y > prevY))
    {
      yield(x-1, y-1, x, y);

      x--;
      y--;
    }

    if(d > 0)
    {
      yield(prevX, prevY, x, y);
    }

    x = prevX;
    y = prevY;


    v = (Array<int>*) m_TraceList.GetPrev();
  }
}

void DiffTrace::yield(int x1, int y1, int x, int y)
{
  const char* aLine = m_A.GetDiffLineText(x1).C_str();
  const char* bLine = m_B.GetDiffLineText(y1).C_str();
  if(x == x1)
  {
    printf("+  %s\n", bLine);
  }
  else if(y == y1)
  {
    printf("-  %s\n", aLine);
  }
  else
  {
    printf("=  %s\n", bLine);
  }
}
