#include <stdio.h>

#include "DiffEngine.h"

DiffEngine::DiffEngine(DiffFileBase& a,
                       DiffFileBase& b,
                       DiffFileBase& aDiff,
                       DiffFileBase& bDiff,
                       bool& bCancelRequested,
                       LinkedList* pDiffStartIdxsList)
  : m_A(a),
    m_B(b),
    m_ADiff(aDiff),
    m_BDiff(bDiff),
    m_NumInsertedB(0),
    m_NumDeletedA(0),
    m_NumChanged(0),
    m_bCancelRequested(bCancelRequested),
    m_pDiffStartIdxsList(pDiffStartIdxsList),
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
  // Set-up the progress reporting
  //
  const long NUM_NOTIFICATIONS = 18;
  long numPositions = m_A.NumLines();
  m_NotifyIncrement = numPositions / NUM_NOTIFICATIONS;
  m_PercentIncrement = 90 / NUM_NOTIFICATIONS;
  m_NextNotifyPosition = m_NotifyIncrement;
  m_Percent = 0;
  reportProgress(0);

  //
  // Calculate some needed values
  //
  m_Max = m_A.NumLines() + m_B.NumLines() + 1;
  m_pDownVector = new long[2 * m_Max + 2];
  m_pUpVector = new long[2 * m_Max + 2];

  m_NumInsertedB = 0;
  m_NumDeletedA = 0;
  m_NumChanged = 0;

  //
  // Calculate the Longest common subsequence
  //   While calculating the lcs the deleted lines in left file m_A are
  //   marked with DiffLine::Deleted and the inserted lines in right
  //   file m_b are marked with DiffLine::Added
  lcs(0, m_A.NumLines(), 0, m_B.NumLines());

  delete[] m_pUpVector;
  m_pUpVector = NULL;

  delete[] m_pDownVector;
  m_pDownVector = NULL;

  if(m_bCancelRequested)
  {
    // Don't continue as cancel was requested
    return false;
  }


  // Progress reporting
  reportProgress(90);

  //
  // Optimizing the diffed files for better diff-readability
  //
  optimize(m_ADiff);
  optimize(m_BDiff);

  // Progress reporting
  reportProgress(95);

  //
  // Calculate the target DiffFilePartitions from src diff files.
  // That means inserting of empty lines in one side when in other
  // side are insertions or Deletions.
  //
  createDiffFiles();

  // Progress reporting
  reportProgress(100);

  return true;
}

long DiffEngine::NumDifferences() const
{
  return m_NumChanged + m_NumDeletedA + m_NumInsertedB;
}


long DiffEngine::NumAdded() const
{
  return m_NumInsertedB;
}


long DiffEngine::NumChanged() const
{
  return m_NumChanged;
}


long DiffEngine::NumDeleted() const
{
  return m_NumDeletedA;
}


void DiffEngine::SetProgressReporter(ProgressReporter* pProgressReporter)
{
  m_pProgressReporter = pProgressReporter;
}

void DiffEngine::addDiffIdxToList(size_t diffIdx)
{
  if(m_pDiffStartIdxsList == NULL)
  {
    return;
  }

  size_t* pItem = new size_t[1];
  pItem[0] = diffIdx;

  m_pDiffStartIdxsList->InsertTail(pItem);
}


void DiffEngine::createDiffFiles()
{
  long lineA = 0;
  long lineB = 0;

  m_ADiff.SetNumLines(m_A.NumLines() + m_NumInsertedB);
  m_BDiff.SetNumLines(m_B.NumLines() + m_NumDeletedA);

  while (lineA < m_A.NumLines() || lineB < m_B.NumLines())
  {
    //
    // Handle the equal lines
    //
    while( (lineA < m_A.NumLines())
     && (m_A.GetLineState(lineA) == DiffLine::Normal)
     && (lineB < m_B.NumLines())
     && (m_B.GetLineState(lineB) == DiffLine::Normal))
    {
      m_ADiff.AddString(m_A.GetLine(lineA++)->Text(), DiffLine::Normal);
      m_BDiff.AddString(m_B.GetLine(lineB++)->Text(), DiffLine::Normal);
    }

    //
    // Handle changed, deleted, inserted lines
    //
    bool bAddedToList = false;
    while((lineA < m_A.NumLines())
      && (lineB < m_B.NumLines())
      && (m_A.GetLineState(lineA) != DiffLine::Normal)
      && (m_B.GetLineState(lineB) != DiffLine::Normal))
    {
      long idx = m_ADiff.AddString(m_A.GetLine(lineA++)->Text(), DiffLine::Changed);
      m_BDiff.AddString(m_B.GetLine(lineB++)->Text(), DiffLine::Changed);

      m_NumChanged++;
      m_NumDeletedA--;
      m_NumInsertedB--;

      m_ADiff.DecrementNumLines();
      m_BDiff.DecrementNumLines();

      // Add start of this block of CHANGED lines to differences list
      if(!bAddedToList)
      {
        bAddedToList = true;
        addDiffIdxToList(idx); // -2 ... has been decremented above
      }
    }

    bAddedToList = false;
    while((lineA < m_A.NumLines())
       && (lineB >= m_B.NumLines() || (m_A.GetLineState(lineA) != DiffLine::Normal)))
    {
      long idx = m_ADiff.AddString(m_A.GetLine(lineA++)->Text(), DiffLine::Deleted);
      m_BDiff.AddBlankLine();

      // Add start of this block of DELETED lines to differences list
      if(!bAddedToList)
      {
        bAddedToList = true;
        addDiffIdxToList(idx);
      }
    }

    bAddedToList = false;
    while((lineB < m_B.NumLines())
      && (lineA >= m_A.NumLines() || (m_B.GetLineState(lineB) != DiffLine::Normal)))
    {
      m_ADiff.AddBlankLine();
      long idx = m_BDiff.AddString(m_B.GetLine(lineB++)->Text(), DiffLine::Added);

      // Add start of this block of ADDED lines to differences list
      if(!bAddedToList)
      {
        bAddedToList = true;
        addDiffIdxToList(idx);
      }
    }

  }
}



void DiffEngine::lcs(long lowerA, long upperA, long lowerB, long upperB)
{
  //
  // Notify
  //
  if((m_NotifyIncrement > 0)
   &&(lowerA > m_NextNotifyPosition))
  {
    if(m_bCancelRequested)
    {
      // Don't continue as cancel was requested
      return;
    }

    m_Percent += m_PercentIncrement;
    m_NextNotifyPosition += m_NotifyIncrement;
    reportProgress(m_Percent);
  }

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
      m_NumInsertedB++;
    }
  }
  else if(lowerB == upperB)
  {
    while(lowerA < upperA)
    {
      m_A.GetLine(lowerA++)->SetState(DiffLine::Deleted);
      m_NumDeletedA++;
    }
  }
  else
  {
    Pair smsrd = sms(lowerA, upperA, lowerB, upperB);
    lcs(lowerA, smsrd.Left(), lowerB, smsrd.Top());

    if(m_bCancelRequested)
    {
      // Don't continue as cancel was requested
      return;
    }

    lcs(smsrd.Left(), upperA, smsrd.Top(), upperB);

    if(m_bCancelRequested)
    {
      // Don't continue as cancel was requested
      return;
    }
  }
}


Pair DiffEngine::sms(long lowerA, long upperA, long lowerB, long upperB)
{
  Pair result;

  // the k-line to start the forward search
  long downK = lowerA - lowerB;

  // the k-line to start the reverse search
  long upK = upperA - upperB;

  long delta = (upperA - lowerA) - (upperB - lowerB);
  bool bOddDelta = (delta & 1) != 0;

  // The vectors in Myers' publication accept negative indexes.
  // The vectors implemented here are 0-based and are accessed using
  // a specific offset: UpOffset for m_pUpVector and DownOffset for
  // m_pDownVector
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
      if (bOddDelta && (upK - D < k) && (k < upK + D))
      {
        if (m_pUpVector[upOffset + k] <= m_pDownVector[downOffset + k])
        {
          result.Set(m_pDownVector[downOffset + k],
                  m_pDownVector[downOffset + k] - k);

          return result;
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
      if (!bOddDelta && (downK - D <= k) && (k <= downK + D))
      {
        if (m_pUpVector[upOffset + k] <= m_pDownVector[downOffset + k])
        {
          result.Set(m_pDownVector[downOffset + k],
                  m_pDownVector[downOffset + k] - k);

          return result;
        }
      }
    }
  }

  // The algorithm should never come here
  Pair resultInvalid;
  return resultInvalid;
}


void DiffEngine::optimize(DiffFileBase& data)
{
  long dataLength = data.NumLines();
  long startPos = 0;
  long endPos = 0;

  while(startPos < data.NumLines())
  {
    while((startPos < dataLength)
       && (data.GetLineState(startPos) == DiffLine::Normal))  // normal
    {
      startPos++;
    }

    endPos = startPos;

    while((endPos < dataLength)
      && (data.GetLineState(startPos) != DiffLine::Normal)) // modified
    {
      endPos++;
    }

    if((endPos < dataLength)
     && (data.GetLine(startPos)->Token() == data.GetLine(endPos)->Token()))
    {
      data.SetLineState(endPos, data.GetLine(startPos)->State());
      data.SetLineState(startPos, DiffLine::Normal);
    }
    else
    {
      startPos = endPos;
    }
  }
}


void DiffEngine::reportProgress(int progress)
{
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(progress);
  }
}
