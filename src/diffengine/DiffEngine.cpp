#include <stdio.h>

#include "DiffEngine.h"

DiffEngine::DiffEngine(DiffFilePartition& a,
                       DiffFilePartition& b,
                       DiffFilePartition& aDiff,
                       DiffFilePartition& bDiff,
                       bool& bCancelRequested)
  : m_A(a),
    m_B(b),
    m_ADiff(aDiff),
    m_BDiff(bDiff),
    m_bCancelRequested(bCancelRequested),
    m_pProgressReporter(NULL),
    m_Max(0),
    m_pDownVector(NULL),
    m_pUpVector(NULL)
{

}

DiffEngine::~DiffEngine()
{

}

bool DiffEngine::Diff()
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

  m_Max = m_A.NumLines() + m_B.NumLines() + 1;
  m_pDownVector = new long[2 * m_Max + 2];
  m_pUpVector = new long[2 * m_Max + 2];

  lcs(0, m_A.NumLines(), 0, m_B.NumLines());

  delete[] m_pUpVector;
  m_pUpVector = NULL;

  delete[] m_pDownVector;
  m_pDownVector = NULL;

  //
  // Calculate the target diff file partitions
  //

  long lineA = 0;
  long lineB = 0;

  while (lineA < m_A.NumLines() || lineB < m_B.NumLines())
  {
    if( (lineA < m_A.NumLines())
     && (m_A.GetLineState(lineA) == DiffLine::Normal)
     && (lineB < m_B.NumLines())
     && (m_B.GetLineState(lineB) == DiffLine::Normal))
    {
      // equal lines
      m_ADiff.AddString(m_A.GetLine(lineA)->Text(), DiffLine::Normal);
      m_BDiff.AddString(m_B.GetLine(lineB)->Text(), DiffLine::Normal);

      lineA++;
      lineB++;
    }
    else
    {
      // maybe deleted and/or inserted lines
      while((lineA < m_A.NumLines())
        && (lineB >= m_B.NumLines() || (m_A.GetLineState(lineA) != DiffLine::Normal)))
      {
        m_ADiff.AddString(m_A.GetLine(lineA)->Text(), m_A.GetLineState(lineA));
        m_BDiff.AddBlankLine();
        lineA++;
      }

      while((lineB < m_B.NumLines())
        && (lineA >= m_A.NumLines() || (m_B.GetLineState(lineB) != DiffLine::Normal)))
      {
        m_ADiff.AddBlankLine();
        m_BDiff.AddString(m_B.GetLine(lineB)->Text(), m_B.GetLineState(lineB));
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


void DiffEngine::lcs(long lowerA,
                     long upperA,
                     long lowerB,
                     long upperB)
{
  // Fast walkthrough equal lines at the start
  while((lowerA < upperA) && (lowerB < upperB)
     && (m_A.GetLine(lowerA)->Token() == m_B.GetLine(lowerB)->Token()))
  {
    lowerA++;
    lowerB++;
  }

  // Fast walkthrough equal lines at the end
  while((lowerA < upperA) && (lowerB < upperB)
     && (m_A.GetLine(upperA - 1)->Token() == m_B.GetLine(upperB - 1)->Token()))
  {
    --upperA;
    --upperB;
  }

  if(lowerA == upperA)
  {
    while(lowerB < upperB)
    {
      m_B.GetLine(lowerB++)->SetState(DiffLine::Added);
    }
  }
  else if(lowerB == upperB)
  {
    while(lowerA < upperA)
    {
      m_A.GetLine(lowerA++)->SetState(DiffLine::Deleted);
    }
  }
  else
  {
    Pair smsrd = shortestMiddleSnake(lowerA, upperA, lowerB, upperB);
    lcs(lowerA, smsrd.Left(), lowerB, smsrd.Top());
    lcs(smsrd.Left(), upperA, smsrd.Top(), upperB);
  }
}



Pair DiffEngine::shortestMiddleSnake(long lowerA,
                                     long upperA,
                                     long lowerB,
                                     long upperB)
{
  Pair sms;

  long downK = lowerA - lowerB; // the k-line to start the forward search
  long upK = upperA - upperB; // the k-line to start the reverse search

  long delta = (upperA - lowerA) - (upperB - lowerB);
  bool oddDelta = (delta & 1) != 0;

  // The vectors in the publication accepts negative indexes.
  // The vectors implemented here are 0-based and are access using
  // a specific offset: UpOffset m_pUpVector and DownOffset for
  // DownVector
  long downOffset = m_Max - downK;
  long upOffset = m_Max - upK;

  long maxD = ((upperA - lowerA + upperB - lowerB) / 2) + 1;

  // init vectors
  m_pDownVector[downOffset + downK + 1] = lowerA;
  m_pUpVector[upOffset + upK - 1] = upperA;

  for (long D = 0; D <= maxD; D++)
  {

    // Extend the forward path
    for (long k = downK - D; k <= downK + D; k += 2)
    {
      // find the only or better starting point
      long x, y;
      if (k == downK - D)
      {
        x = m_pDownVector[downOffset + k + 1]; // down
      }
      else
      {
        x = m_pDownVector[downOffset + k - 1] + 1; // a step to the right

        if ((k < downK + D) && (m_pDownVector[downOffset + k + 1] >= x))
        {
          x = m_pDownVector[downOffset + k + 1]; // down
        }
      }
      y = x - k;

      // find the end of the furthest reaching forward D-path in diagonal k.
      while ((x < upperA) && (y < upperB)
          && (m_A.GetLine(x)->Token() == m_B.GetLine(y)->Token()))
          //&& (m_A.GetLine(x)->Text() == m_B.GetLine(y)->Text()))
      {
        x++;
        y++;
      }

      m_pDownVector[downOffset + k] = x;

      // overlap ?
      if (oddDelta && (upK - D < k) && (k < upK + D))
      {
        if (m_pUpVector[upOffset + k] <= m_pDownVector[downOffset + k])
        {
          sms.Set(m_pDownVector[downOffset + k],
                  m_pDownVector[downOffset + k] - k);

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
        x = m_pUpVector[upOffset + k - 1]; // up
      }
      else
      {
        x = m_pUpVector[upOffset + k + 1] - 1; // left

        if ((k > upK - D) && (m_pUpVector[upOffset + k - 1] < x))
        {
          x = m_pUpVector[upOffset + k - 1]; // up
        }
      }

      y = x - k;

      while ((x > lowerA) && (y > lowerB)
          && (m_A.GetLine(x - 1)->Token() == m_B.GetLine(y - 1)->Token()))
          //&& (m_A.GetLine(x - 1)->Text() == m_B.GetLine(y - 1)->Text()))
      {
        // diagonal
        x--;
        y--;
      }

      m_pUpVector[upOffset + k] = x;

      // overlap ?
      if (!oddDelta && (downK - D <= k) && (k <= downK + D))
      {
        if (m_pUpVector[upOffset + k] <= m_pDownVector[downOffset + k])
        {
          sms.Set(m_pDownVector[downOffset + k],
                  m_pDownVector[downOffset + k] - k);

          return sms;
        }
      }

    }

  }

  // The algorithm should never come here
  Pair smsNil;
  return smsNil;
}
