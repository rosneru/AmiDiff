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

  //
  // Calculate the Longest common subsequence
  //

  m_Max = srcA.NumLines() + srcB.NumLines() + 1;
  long* pDownVector = new long[2 * m_Max + 2];
  long* pUpVector = new long[2 * m_Max + 2];

  lcs(srcA, 0, srcA.NumLines(), srcB, 0, srcB.NumLines(), pDownVector, pUpVector);

  delete[] pUpVector;
  delete[] pDownVector;

  //
  // Calculate the target diff file partitions
  //

  long lineA = 0;
  long lineB = 0;

  while (lineA < srcA.NumLines() || lineB < srcB.NumLines())
  {
    if( (lineA < srcA.NumLines())
     && (srcA.GetLineState(lineA) == DiffLine::Normal)
     && (lineB < srcB.NumLines())
     && (srcB.GetLineState(lineB) == DiffLine::Normal))
    {
      // equal lines
      diffA.AddString(srcA.GetLineText(lineA), DiffLine::Normal);
      diffB.AddString(srcB.GetLineText(lineB), DiffLine::Normal);

      lineA++;
      lineB++;
    }
    else
    {
      // maybe deleted and/or inserted lines
      while((lineA < srcA.NumLines())
        && (lineB >= srcB.NumLines() || (srcA.GetLineState(lineA) != DiffLine::Normal)))
      {
        diffA.AddString(srcA.GetLineText(lineA), srcA.GetLineState(lineA));
        diffB.AddBlankLine();
        lineA++;
      }

      while((lineB < srcB.NumLines())
        && (lineA >= srcA.NumLines() || (srcB.GetLineState(lineB) != DiffLine::Normal)))
      {
        diffA.AddBlankLine();
        diffB.AddString(srcB.GetLineText(lineB), srcB.GetLineState(lineB));
        lineB++;
      }
    }
  }

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


void DiffEngine::lcs(DiffFilePartition& a,
                     long lowerA,
                     long upperA,
                     DiffFilePartition& b,
                     long lowerB,
                     long upperB,
                     long* pDownVector,
                     long* pUpVector)
{
  // Fast walkthrough equal lines at the start
  while((lowerA < upperA) && (lowerB < upperB)
     && (a.GetLineToken(lowerA) == b.GetLineToken(lowerB)))
  {
    lowerA++;
    lowerB++;
  }

  // Fast walkthrough equal lines at the end
  while((lowerA < upperA) && (lowerB < upperB)
     && (a.GetLineToken(upperA - 1) == b.GetLineToken(upperB - 1)))
  {
    --upperA;
    --upperB;
  }

  if(lowerA == upperA)
  {
    while(lowerB < upperB)
    {
      b.GetLine(lowerB++)->SetState(DiffLine::Added);
    }
  }
  else if(lowerB == upperB)
  {
    while(lowerA < upperA)
    {
      a.GetLine(lowerA++)->SetState(DiffLine::Deleted);
    }
  }
  else
  {
    Pair smsrd = shortestMiddleSnake(a, lowerA, upperA, b, lowerB, upperB, pDownVector, pUpVector);
    lcs(a, lowerA, smsrd.Left(), b, lowerB, smsrd.Top(), pDownVector, pUpVector);
    lcs(a, smsrd.Left(), upperA, b, smsrd.Top(), upperB, pDownVector, pUpVector);
  }
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
          && (a.GetLineToken(x) == b.GetLineToken(y))
          && (a.GetLineText(x) == b.GetLineText(y)))
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
          && (a.GetLineToken(x - 1) == b.GetLineToken(y - 1))
          && (a.GetLineText(x - 1) == b.GetLineText(y - 1)))
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
