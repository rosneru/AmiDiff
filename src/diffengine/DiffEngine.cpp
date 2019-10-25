#include <stdio.h>

#include "DiffEngine.h"

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
                      DiffFilePartition& targetA,
                      DiffFilePartition& targetB)
{
//  Trace trace(srcA, srcB, targetA, targetB);
//  if(shortestEdit(trace, srcA, srcB) == false)
//  {
//    return false;
//  }

  //
  // Progress reporting
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(50);
  }

  Array<Point>* pPath = findPath(0, 0, srcA.NumLines(), srcB.NumLines(), srcA, srcB);
  if(pPath->Size() == 0)
  {
    return false;
  }

  printf("[ ");
  for(int i = 0; i < pPath->Size(); i++)
  {
    Point box = (*pPath)[i];
    printf("[%d, %d], ", box.Left(), box.Top());
    if(i < pPath->Size() - 1)
    {
      printf(", \n  ");
    }
    else
    {
      printf(" ]\n");
    }
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


Array<Point>* DiffEngine::findPath(long left, long top, long right, long bottom, DiffFilePartition& a, DiffFilePartition& b)
{
  Box box(left, top, right, bottom);
  Box snake = midpoint(box, a, b);

  printf("Midpoint of [%d, %d], [%d, %d]]", left, top, right, bottom);

  if(snake.Size() > 0)
  {
    printf(" --> Snake is: [[%d, %d], [%d, %d]]\n", snake.Left(), snake.Top(), snake.Right(), snake.Bottom());
  }
  else
  {
    printf("Snake is: \n");
  }


  if(snake.Size() == 0)
  {
    return new Array<Point>();
  }

  Array<Point>* pHead = findPath(left, top, snake.Left(), snake.Top(), a, b);
  Array<Point>* pTail = findPath(snake.Right(), snake.Bottom(), right, bottom, a, b);

  if(pHead->Size() > 0)
  {
    if(pTail->Size() > 0)
    {
      for(int i = 0; i < pTail->Size(); i++)
      {
        pHead->Push((*pTail)[i]);
      }

      delete pTail;
      return pHead;
    }
    else
    {
      pHead->Push(Point(snake.Right(), snake.Bottom()));

      delete pTail;
      return pHead;
    }
  }
  else
  {
    Array<Point>* pResult = new Array<Point>();
    pResult->Push(Point(snake.Left(), snake.Top()));

    if(pTail->Size() > 0)
    {
      for(int i = 0; i < pTail->Size(); i++)
      {
        pResult->Push((*pTail)[i]);
      }

      delete pHead;
      delete pTail;
      return pResult;
    }
    else
    {
      pResult->Push(Point(snake.Right(), snake.Bottom()));

      delete pHead;
      delete pTail;
      return pResult;
    }
  }
}


Box DiffEngine::midpoint(Box box, DiffFilePartition& a, DiffFilePartition& b)
{
  if(box.Size() == 0)
  {
    // If this box is empty return the empty box to signal the failure
    return box;
  }

  // Original: max = (box.size / 2.0).ceil
  int max = (box.Size() / 2) + ((box.Size() % 2) != 0);

  int vSize = 2 * max + 1;

  int* vf = new int[vSize];
  vf[1] = box.Left();

  int* vb = new int[vSize];
  vb[1] = box.Bottom();

  for(int d = 0; d <= max; d++)
  {
    Box forwardSnake = forwards(box, vf, vb, vSize, d, a, b);
    if(forwardSnake.Size() > 0)
    {
      delete[] vf;
      delete[] vb;
      return forwardSnake;
    }

    Box backwardSnake = backward(box, vf, vb, vSize, d, a, b);
    if(backwardSnake.Size() > 0)
    {
      delete[] vf;
      delete[] vb;
      return backwardSnake;
    }
  }

  delete[] vf;
  delete[] vb;
  Box result(0, 0, 0, 0);
  return result;
}

Box DiffEngine::forwards(Box box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
{
  int x, px, y, py;

  for(long k = d; k >= -d; k -= 2)
  {
    long c = k - box.Delta();

    if((k == -d) || ((k != d) && (vf[Trace::IdxConv(k - 1, vSize)] < vf[Trace::IdxConv(k + 1, vSize)])))
    {
      x = px = vf[Trace::IdxConv(k + 1, vSize)];
    }
    else
    {
      px = vf[Trace::IdxConv(k - 1, vSize)];
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
       && (a.GetDiffLine(x)->Text() == b.GetDiffLine(y)->Text()))
    {
      x++;
      y++;
    }

    int idx = Trace::IdxConv(k, vSize);
    vf[idx] = x;

    if( ((box.Delta() % 2) != 0)  // true if box.Delta() is odd
     && (Trace::Between(c, -(d - 1), d - 1))
     && (y >= vb[c]))
    {
      // yield [[px, py], [x, y]]
      // TODO
      Box result(px, py, x, y);
      return result;
    }
  }

  Box result(0, 0, 0, 0);
  return result;
}

Box DiffEngine::backward(Box box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
{
  int x, px, y, py;

  for(long c = d; c >= -d; c -= 2)
  {
    long k = c + box.Delta();

    if((c == -d) || ((c != d) && (vb[Trace::IdxConv(c - 1, vSize)] > vb[Trace::IdxConv(c + 1, vSize)])))
    {
      y = py = vb[Trace::IdxConv(c + 1, vSize)];
    }
    else
    {
      py = vb[Trace::IdxConv(c - 1, vSize)];
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
       && (a.GetDiffLine(x - 1)->Text() == b.GetDiffLine(y - 1)->Text()))
    {
      x--;
      y--;
    }

    int idx = Trace::IdxConv(c, vSize);
    vb[idx] = y;

    if(k > (vSize - 1))
    {
      printf("ILLEGAL ACCESS\n");
    }

    // %2 => true if box.Delta() is even
    if( ((box.Delta() % 2) == 0) && (Trace::Between(k, -d, d)) && (x <= vf[k]))
    {
      // yield [[x, y], [px, py]]
      // TODO
      Box result(x, y, px, py);
      return result;
    }
  }

  Box result(0, 0, 0, 0);
  return result;
}
