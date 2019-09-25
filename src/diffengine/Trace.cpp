#include <stdio.h>

#include "Trace.h"

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
  while(m_TracesArray.Size() > 0)
  {
    Array<int>* pItem = NULL;
    if((pItem = (Array<int>*) m_TracesArray.Pop()) != NULL)
    {
      delete pItem;
    }
  }

}


void DiffTrace::AddTrace(const int* array, size_t arrayLen)
{
  Array<int>* pItem = new Array<int>();

  for(size_t i = 0; i < arrayLen; i++)
  {
    int value = array[i];
    pItem->Push(value);
  }

  m_TracesArray.Push(pItem);
}


void DiffTrace::Backtrack()
{
  int x = m_SrcA.NumLines();
  int y = m_SrcB.NumLines();

  int prevK;
  int prevX;
  int prevY;


  int i = m_TracesArray.Size() - 1;
  Array<int>* v = m_TracesArray[i];
  for(int d = m_TracesArray.Size() - 1; d >= 0; d--)
  {
    if(v == NULL)
    {
      break;
    }

    int vSize = v->Size();

    int k = x - y;

    if((k == -d) || ((k != d)
      && ((*v)[IdxConv(k - 1, vSize)] < (*v)[IdxConv(k + 1, vSize)])))
    {
      prevK = k + 1;
    }
    else
    {
      prevK = k - 1;
    }

    int idx = IdxConv(prevK, vSize);
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

    i--;
    if(i < 0)
    {
      break;
    }

    v = m_TracesArray[i];
  }
}



size_t DiffTrace::IdxConv(int idx, int arraySize)
{
  if(idx < 0)
  {
    if(idx + arraySize < 0)
    {
      int factor = -idx / arraySize;
      idx = idx + factor * arraySize;
    }

    return arraySize + idx;
  }

  if(idx >= arraySize)
  {
    if(-idx + arraySize <= 0)
    {
      int factor = idx / arraySize;
      idx = idx - factor * arraySize;
    }

    return idx;
  }

  return idx;
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

