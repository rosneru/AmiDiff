#include <stdio.h>

#include "DiffTrace.h"

DiffTrace::DiffTrace(DiffFilePartition& srcA,
                     DiffFilePartition& srcB,
                     DiffFilePartition& targetA,
                     DiffFilePartition& targetB)
  : m_SrcA(srcA),
    m_SrcB(srcB),
    m_TargetA(targetA),
    m_TargetB(targetB)
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


void DiffTrace::Backtrack()
{
  int x = m_SrcA.NumLines();
  int y = m_SrcB.NumLines();

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

    if((k == -d) || ((k != d)
      && ((*v)[IdxConv(k - 1, 1, vSize)] < (*v)[IdxConv(k + 1, 1, vSize)])))
    {
      prevK = k + 1;
    }
    else
    {
      prevK = k - 1;
    }

    int idx = IdxConv(prevK, 1, vSize);
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



int DiffTrace::IdxConv(int i, int from, int to)
{
  if((i + from) < from)
  {
    return from + to + i;
  }

  if(i > to)
  {
    return from + (i - to);
  }

  return i;
}


Array<int>* DiffTrace::firstTrace()
{
  return (Array<int>*) m_TraceList.GetFirst();
}


Array<int>* DiffTrace::nextTrace()
{
  return (Array<int>*) m_TraceList.GetNext();
}

size_t DiffTrace::numTraces()
{
  return m_TraceList.Size();
}


void DiffTrace::yield(int x1, int y1, int x, int y)
{
//  const char* pLineA = m_SrcA.GetDiffLineText(x1).C_str();
//  const char* pLineB = m_SrcB.GetDiffLineText(y1).C_str();
  if(x == x1)
  {
//    printf("+  %s\n", pLineB);
    m_TargetA.AddBlankLine();
    m_TargetB.AddString(m_SrcB.GetDiffLineText(y1), DiffLine::Added);
  }
  else if(y == y1)
  {
//    printf("-  %s\n", pLineA);
    m_TargetA.AddString(m_SrcA.GetDiffLineText(x1), DiffLine::Deleted);
    m_TargetB.AddBlankLine();
  }
  else
  {
//    printf("=  %s\n", pLineA);
    m_TargetA.AddString(m_SrcA.GetDiffLineText(x1), DiffLine::Normal);
    m_TargetB.AddString(m_SrcB.GetDiffLineText(y1), DiffLine::Normal);
  }
}

