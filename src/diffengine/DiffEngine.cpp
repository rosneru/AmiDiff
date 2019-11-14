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
    m_pProgressReporter(NULL),
    m_Max(0)
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

  m_Max = srcA.NumLines() + srcB.NumLines() + 1;
  long* pDownVector = new long[2 * m_Max + 2];
  long* pUpVector = new long[2 * m_Max + 2];

  FindPath(srcA, 0, srcA.NumLines(), srcB, 0, srcB.NumLines(), pDownVector, pUpVector);

  delete[] pUpVector;
  delete[] pDownVector;

  //
  // Calculate the target diff file partitions
  //
  long startA;
  long startB;
  long lineA = 0;
  long lineB = 0;

  while (lineA < srcA.NumLines() || lineB < srcB.NumLines())
  {
    if( (lineA < srcA.NumLines())
     && (srcA.GetDiffLineState(lineA) == DiffLine::Normal)
     && (lineB < srcB.NumLines())
     && (srcB.GetDiffLineState(lineB) == DiffLine::Normal))
    {
      // equal lines
      diffA.AddString(srcA.GetDiffLineText(lineA), DiffLine::Normal);
      diffB.AddString(srcB.GetDiffLineText(lineB), DiffLine::Normal);

      lineA++;
      lineB++;
    }
    else
    {
      // maybe deleted and/or inserted lines
      startA = lineA;
      startB = lineB;

      while((lineA < srcA.NumLines())
        && (lineB >= srcB.NumLines() || (srcA.GetDiffLineState(lineA) != DiffLine::Normal)))
      {
        diffA.AddString(srcA.GetDiffLineText(lineA), srcA.GetDiffLineState(lineA));
        diffB.AddBlankLine();
        lineA++;
      }

      while((lineB < srcB.NumLines())
        && (lineA >= srcA.NumLines() || (srcB.GetDiffLineState(lineB) != DiffLine::Normal)))
      {
        diffA.AddBlankLine();
        diffB.AddString(srcB.GetDiffLineText(lineB), srcB.GetDiffLineState(lineB));
        lineB++;
      }

//      if ((startA < lineA) || (startB < lineB))
//      {
//        // store a new difference-item
//           //   aItem = new DiffItem();
//           //   aItem.StartLeft = StartA;
//           //   aItem.StartRight = StartB;
//           //   aItem.DeletedLeft = LineA - StartA;
//           //   aItem.InsertedRight = LineB - StartB;
//           //   a.Add(aItem);
//      }
    }
  }

  /*
  LinkedList* pPath = FindPath(srcA, 0, 0, srcB, srcA.NumLines(), srcB.NumLines());
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

  */

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

Pair DiffEngine::shortestMiddleSnake(DiffFilePartition& a,
                                     long lowerA,
                                     long upperA,
                                     DiffFilePartition& b,
                                     long lowerB,
                                     long upperB,
                                     long* pDownVector,
                                     long* pUpVector)
{
  Pair sms;

  long downK = lowerA - lowerB; // the k-line to start the forward search
  long upK = upperA - upperB; // the k-line to start the reverse search

  long delta = (upperA - lowerA) - (upperB - lowerB);
  bool oddDelta = (delta & 1) != 0;

  // The vectors in the publication accepts negative indexes.
  // The vectors implemented here are 0-based and are access using
  // a specific offset: UpOffset pUpVector and DownOffset for
  // DownVector
  long downOffset = m_Max - downK;
  long upOffset = m_Max - upK;

  long maxD = ((upperA - lowerA + upperB - lowerB) / 2) + 1;

  // init vectors
  pDownVector[downOffset + downK + 1] = lowerA;
  pUpVector[upOffset + upK - 1] = upperA;

  for (long D = 0; D <= maxD; D++)
  {

    // Extend the forward path
    for (long k = downK - D; k <= downK + D; k += 2)
    {
      // find the only or better starting point
      long x, y;
      if (k == downK - D)
      {
        x = pDownVector[downOffset + k + 1]; // down
      }
      else
      {
        x = pDownVector[downOffset + k - 1] + 1; // a step to the right

        if ((k < downK + D) && (pDownVector[downOffset + k + 1] >= x))
        {
          x = pDownVector[downOffset + k + 1]; // down
        }
      }
      y = x - k;

      // find the end of the furthest reaching forward D-path in diagonal k.
      while ((x < upperA) && (y < upperB)
          && (a.GetDiffLine(x)->Token() == b.GetDiffLine(y)->Token())
          && (a.GetDiffLineText(x) == b.GetDiffLineText(y)))
      {
        x++;
        y++;
      }

      pDownVector[downOffset + k] = x;

      // overlap ?
      if (oddDelta && (upK - D < k) && (k < upK + D))
      {
        if (pUpVector[upOffset + k] <= pDownVector[downOffset + k])
        {
          sms.Set(pDownVector[downOffset + k],
                  pDownVector[downOffset + k] - k);
          return sms;
        }
      }

    }

    // Extend the reverse path.
    for (long k = upK - D; k <= upK + D; k += 2)
    {
      // find the only or better starting point
      long x, y;
      if (k == upK + D)
      {
        x = pUpVector[upOffset + k - 1]; // up
      }
      else
      {
        x = pUpVector[upOffset + k + 1] - 1; // left

        if ((k > upK - D) && (pUpVector[upOffset + k - 1] < x))
        {
          x = pUpVector[upOffset + k - 1]; // up
        }
      }

      y = x - k;

      while ((x > lowerA) && (y > lowerB)
          && (a.GetDiffLine(x - 1)->Token() == b.GetDiffLine(y - 1)->Token())
          && (a.GetDiffLineText(x - 1) == b.GetDiffLineText(y - 1)))
      {
        // diagonal
        x--;
        y--;
      }

      pUpVector[upOffset + k] = x;

      // overlap ?
      if (!oddDelta && (downK - D <= k) && (k <= downK + D))
      {
        if (pUpVector[upOffset + k] <= pDownVector[downOffset + k])
        {
          sms.Set(pDownVector[downOffset + k],
                  pDownVector[downOffset + k] - k);

          return sms;
        }
      }

    }

  }

  // The algorithm should never come here
  Pair nil;
  return nil;
}


void DiffEngine::FindPath(DiffFilePartition& a,
                          long left,
                          long top,
                          DiffFilePartition& b,
                          long right,
                          long bottom,
                          long* pDownVector,
                          long* pUpVector)
{
//  Box snake(left, top, right, bottom);
//  bool bFoundSnake = midPair(snake, a, b);

//  if(!bFoundSnake)
//  {
//    return new LinkedList();
//  }

  while((left < top) && (right < bottom)
     && (a.GetDiffLine(left)->Token() == b.GetDiffLine(right)->Token()))
  {
    --left;
    --right;
  }

  if(left == top)
  {
    while(right < bottom)
    {
      b.GetDiffLine(right++)->SetState(DiffLine::Added);
    }
  }
  else if(right == bottom)
  {
    while(left < top)
    {
      a.GetDiffLine(left++)->SetState(DiffLine::Deleted);
    }
  }
  else
  {
    Pair smsrd = shortestMiddleSnake(a, left, top, b, right, bottom, pDownVector, pUpVector);
    FindPath(a, left, smsrd.Left(), b, right, smsrd.Top(), pDownVector, pUpVector);
    FindPath(a, smsrd.Left(), top, b, smsrd.Top(), bottom, pDownVector, pUpVector);
  }
}


//bool DiffEngine::midPair(Box& box, DiffFilePartition& a, DiffFilePartition& b)
//{
//  if(box.Size() == 0)
//  {
//    // If this box is empty return the empty box to signal the failure
//    return false;
//  }

//  // Original: max = (box.size / 2.0).ceil
//  int max = (box.Size() + 1) / 2;

//  int vSize = 2 * max + 1;

//  int* vf = new int[vSize];
//  vf[1] = box.Left();

//  int* vb = new int[vSize];
//  vb[1] = box.Bottom();

//  for(int d = 0; d <= max; d++)
//  {
//    bool bFoundForwards = forwards(box, vf, vb, vSize, d, a, b);
//    if(bFoundForwards)
//    {
//      delete[] vf;
//      delete[] vb;
//      return true;
//    }

//    bool bFoundBackwards = backward(box, vf, vb, vSize, d, a, b);
//    if(bFoundBackwards)
//    {
//      delete[] vf;
//      delete[] vb;
//      return true;
//    }
//  }

//  delete[] vf;
//  delete[] vb;

//  return false;
//}

//bool DiffEngine::forwards(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
//{
//  int x, px, y, py;

//  for(long k = d; k >= -d; k -= 2)
//  {
//    long c = k - box.Delta();

//    if((k == -d) || ((k != d) && (vf[IdxConv(k - 1, vSize)] < vf[IdxConv(k + 1, vSize)])))
//    {
//      x = px = vf[IdxConv(k + 1, vSize)];
//    }
//    else
//    {
//      px = vf[IdxConv(k - 1, vSize)];
//      x = px + 1;
//    }

//    y = box.Top() + (x - box.Left()) - k;

//    if((d == 0) || (x != px))
//    {
//      py = y;
//    }
//    else
//    {
//      py = y - 1;
//    }

//    while((x < box.Right())
//       && (y < box.Bottom())
//       && (a.GetDiffLine(x)->Token() == b.GetDiffLine(y)->Token())
//       && (a.GetDiffLineText(x) == b.GetDiffLineText(y)))
//    {
//      x++;
//      y++;
//    }

//    int cId = IdxConv(c, vSize);
//    int kId = IdxConv(k, vSize);

//    vf[kId] = x;

//    if( ((box.Delta() & 1) == 1)  // true if box.Delta() is odd
//     && (Between(c, -(d - 1), d - 1))
//     && (y >= vb[cId]))
//    {
//      box.Set(px, py, x, y);
//      return true;
//    }
//  }

//  return false;
//}

//bool DiffEngine::backward(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b)
//{
//  int x, px, y, py;

//  for(long c = d; c >= -d; c -= 2)
//  {
//    long k = c + box.Delta();

//    if((c == -d) || ((c != d) && (vb[IdxConv(c - 1, vSize)] > vb[IdxConv(c + 1, vSize)])))
//    {
//      y = py = vb[IdxConv(c + 1, vSize)];
//    }
//    else
//    {
//      py = vb[IdxConv(c - 1, vSize)];
//      y = py - 1;
//    }

//    x = box.Left() + (y - box.Top()) + k;

//    if((d == 0) || (y != py))
//    {
//      px = x;
//    }
//    else
//    {
//      px = x + 1;
//    }

//    while((x > box.Left())
//       && (y > box.Top())
//       && (a.GetDiffLine(x - 1)->Token() == b.GetDiffLine(y - 1)->Token())
//       && (a.GetDiffLineText(x - 1) == b.GetDiffLineText(y - 1)))
//    {
//      x--;
//      y--;
//    }

//    int cId = IdxConv(c, vSize);
//    int kId = IdxConv(k, vSize);
//    vb[cId] = y;

//    // &1 == 0 => true if box.Delta() is even
//    if( ((box.Delta() & 1) == 0) && (Between(k, -d, d)) && (x <= vf[kId]))
//    {
//      // yield [[x, y], [px, py]]
//      // TODO
//      box.Set(x, y, px, py);
//      return true;
//    }
//  }

//  return false;
//}

//void DiffEngine::buildDiff(int x1, int y1, int x2, int y2,
//                           DiffFilePartition& srcA,
//                           DiffFilePartition& srcB,
//                           DiffFilePartition& diffA,
//                           DiffFilePartition& diffB)
//{
//  if(x1 == x2)
//  {
//    // srcB[y1] is an *inserted* line
//    diffA.AddBlankLine();
//    diffB.AddString(srcB.GetDiffLineText(y1), DiffLine::Added);
//  }
//  else if(y1 == y2)
//  {
//    // srcA[x1] is a deleted line
//    diffA.AddString(srcA.GetDiffLineText(x1), DiffLine::Deleted);
//    diffB.AddBlankLine();
//  }
//  else
//  {
//    // line is equal in srcA and srcB
//    diffA.AddString(srcA.GetDiffLineText(x1), DiffLine::Normal);
//    diffB.AddString(srcB.GetDiffLineText(y1), DiffLine::Normal);
//  }
//}
