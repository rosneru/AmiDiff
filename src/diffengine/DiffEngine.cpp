#include <stdio.h>

#include "DiffEngine.h"

/**
 * @brief
 * Helper method!
 *
 * Convert the given index into a 'Ruby-like-array-index' in regards to
 * the target-array's dimension (from- and to-value).
 *
 * That means the target arrays index will not be exceeded or deceeded.
 * When due to exceeding the remaining 'id-portion' is added from the
 * start, when due to deceeding it's subtracted from the end.
 */
inline size_t IdxConv(int idx, int arraySize)
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

inline bool Between(long number, long min, long max)
{
  if((number < min) || (number > max))
  {
    return false;
  }

  return true;
}


DiffEngine::DiffEngine(bool& bCancelRequested)
  : m_bCancelRequested(bCancelRequested),
    m_pProgressReporter(NULL)
{

}

DiffEngine::~DiffEngine()
{

}

bool DiffEngine::Diff(DiffFilePartition& srcA,
                      DiffFilePartition& srcB,
                      DiffFilePartition& diffA,
                      DiffFilePartition& diffB)
{
  //
  // Progress reporting
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(50);
  }

  LinkedList* pPath = FindPath(0, 0, srcA.NumLines(), srcB.NumLines(), srcA, srcB);
  if(pPath->Size() == 0)
  {
    return false;
  }

  Pair* pItem = (Pair*)pPath->GetFirst();
  int x1 = pItem->Left();
  int y1 = pItem->Top();
  delete pItem;


  while((pItem = (Pair*)pPath->GetNext()) != NULL)
  {
    int x2 = pItem->Left();
    int y2 = pItem->Top();


    // Walk diagonal #1
    while((x1 < x2)
       && (y1 < y2)
       && (srcA.GetDiffLine(x1)->Token() == srcB.GetDiffLine(y1)->Token())
       && (srcA.GetDiffLineText(x1) == srcB.GetDiffLineText(y1)))    // TODO remove and run the tests
    {
      buildDiff(x1, y1, x1 + 1, y1 + 1, srcA, srcB, diffA, diffB);
      x1++;
      y1++;
    }

    int dx = x2 - x1;
    int dy = y2 - y1;

    if(dx < dy)
    {
      buildDiff(x1, y1, x1, y1 + 1, srcA, srcB, diffA, diffB);
      y1++;
    }
    else if(dx > dy)
    {
      buildDiff(x1, y1, x1 + 1, y1, srcA, srcB, diffA, diffB);
    }

    // Walk diagonal #2
    while((x1 < x2)
       && (y1 < y2)
       && (srcA.GetDiffLine(x1)->Token() == srcB.GetDiffLine(y1)->Token())
       && (srcA.GetDiffLineText(x1) == srcB.GetDiffLineText(y1)))    // TODO remove and run the tests
    {
      buildDiff(x1, y1, x1 + 1, y1 + 1, srcA, srcB, diffA, diffB);
      x1++;
      y1++;
    }

    x1 = x2;
    y1 = y2;
    delete pItem;
  }

  delete pPath;

  //
  // Progress reporting
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(100);
  }


  return true;
}



void DiffEngine::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}


LinkedList* DiffEngine::FindPath(long left, long top, long right, long bottom, DiffFilePartition& a, DiffFilePartition& b)
{
  Box snake(left, top, right, bottom);
  bool bFoundSnake = midPair(snake, a, b);

  if(!bFoundSnake)
  {
    return new LinkedList();
  }

  LinkedList* pHead = FindPath(left, top, snake.Left(), snake.Top(), a, b);
  LinkedList* pTail = FindPath(snake.Right(), snake.Bottom(), right, bottom, a, b);

  if(pHead->Size() > 0)
  {
    if(pTail->Size() > 0)
    {
      Pair* pItem = (Pair*)pTail->GetFirst();
      do
      {
        pHead->InsertTail(pItem);
      }
      while((pItem = (Pair*)pTail->GetNext()) != NULL);

      delete pTail;
      return pHead;
    }
    else
    {
      pHead->InsertTail(new Pair(snake.Right(), snake.Bottom()));

      delete pTail;
      return pHead;
    }
  }
  else
  {
    if(pTail->Size() > 0)
    {
      pTail->InsertHead(new Pair(snake.Left(), snake.Top()));

      delete pHead;
      return pTail;
    }
    else
    {
      pTail->InsertTail(new Pair(snake.Left(), snake.Top()));
      pTail->InsertTail(new Pair(snake.Right(), snake.Bottom()));

      delete pHead;
      return pTail;
    }
  }
}


bool DiffEngine::midPair(Box& box, DiffFilePartition& a, DiffFilePartition& b)
{
  if(box.Size() == 0)
  {
    // If this box is empty return the empty box to signal the failure
    return false;
  }

  // Original: max = (box.size / 2.0).ceil
  int max = (box.Size() + 1) / 2;

  int vSize = 2 * max + 1;

  int* vf = new int[vSize];
  vf[1] = box.Left();

  int* vb = new int[vSize];
  vb[1] = box.Bottom();

  for(int d = 0; d <= max; d++)
  {
    bool bFoundForwards = forwards(box, vf, vb, vSize, d, a, b);
    if(bFoundForwards)
    {
      delete[] vf;
      delete[] vb;
      return true;
    }

    bool bFoundBackwards = backward(box, vf, vb, vSize, d, a, b);
    if(bFoundBackwards)
    {
      delete[] vf;
      delete[] vb;
      return true;
    }
  }

  delete[] vf;
  delete[] vb;

  return false;
}

bool DiffEngine::forwards(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
{
  int x, px, y, py;

  for(long k = d; k >= -d; k -= 2)
  {
    long c = k - box.Delta();

    if((k == -d) || ((k != d) && (vf[IdxConv(k - 1, vSize)] < vf[IdxConv(k + 1, vSize)])))
    {
      x = px = vf[IdxConv(k + 1, vSize)];
    }
    else
    {
      px = vf[IdxConv(k - 1, vSize)];
      x = px + 1;
    }

    y = box.Top() + (x - box.Left()) - k;

    if((d == 0) || (x != px))
    {
      py = y;
    }
    else
    {
      py = y - 1;
    }

    while((x < box.Right())
       && (y < box.Bottom())
       && (a.GetDiffLine(x)->Token() == b.GetDiffLine(y)->Token())
       && (a.GetDiffLineText(x) == b.GetDiffLineText(y)))
    {
      x++;
      y++;
    }

    int cId = IdxConv(c, vSize);
    int kId = IdxConv(k, vSize);

    vf[kId] = x;

    if( ((box.Delta() & 1) == 1)  // true if box.Delta() is odd
     && (Between(c, -(d - 1), d - 1))
     && (y >= vb[cId]))
    {
      box.Set(px, py, x, y);
      return true;
    }
  }

  return false;
}

bool DiffEngine::backward(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
{
  int x, px, y, py;

  for(long c = d; c >= -d; c -= 2)
  {
    long k = c + box.Delta();

    if((c == -d) || ((c != d) && (vb[IdxConv(c - 1, vSize)] > vb[IdxConv(c + 1, vSize)])))
    {
      y = py = vb[IdxConv(c + 1, vSize)];
    }
    else
    {
      py = vb[IdxConv(c - 1, vSize)];
      y = py - 1;
    }

    x = box.Left() + (y - box.Top()) + k;

    if((d == 0) || (y != py))
    {
      px = x;
    }
    else
    {
      px = x + 1;
    }

    while((x > box.Left())
       && (y > box.Top())
       && (a.GetDiffLine(x - 1)->Token() == b.GetDiffLine(y - 1)->Token())
       && (a.GetDiffLineText(x - 1) == b.GetDiffLineText(y - 1)))
    {
      x--;
      y--;
    }

    int cId = IdxConv(c, vSize);
    int kId = IdxConv(k, vSize);
    vb[cId] = y;

    // &1 == 0 => true if box.Delta() is even
    if( ((box.Delta() & 1) == 0) && (Between(k, -d, d)) && (x <= vf[kId]))
    {
      // yield [[x, y], [px, py]]
      // TODO
      box.Set(x, y, px, py);
      return true;
    }
  }

  return false;
}

void DiffEngine::buildDiff(int x1, int y1, int x2, int y2,
                           DiffFilePartition& srcA,
                           DiffFilePartition& srcB,
                           DiffFilePartition& diffA,
                           DiffFilePartition& diffB)
{
  if(x1 == x2)
  {
    // srcB[y1] is an *inserted* line
    diffA.AddBlankLine();
    diffB.AddString(srcB.GetDiffLineText(y1), DiffLine::Added);
  }
  else if(y1 == y2)
  {
    // srcA[x1] is a deleted line
    diffA.AddString(srcA.GetDiffLineText(x1), DiffLine::Deleted);
    diffB.AddBlankLine();
  }
  else
  {
    // line is equal in srcA and srcB
    diffA.AddString(srcA.GetDiffLineText(x1), DiffLine::Normal);
    diffB.AddString(srcB.GetDiffLineText(y1), DiffLine::Normal);
  }
}
