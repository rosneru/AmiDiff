#include <stdio.h>

#include "DiffEngine.h"

DiffEngine::DiffEngine(DiffInputFileBase& aIn,
                       DiffInputFileBase& bIn,
                       DiffOutputFileBase& aOut,
                       DiffOutputFileBase& bOut,
                       ProgressReporter& progress,
                       const char* pProgressDescription,
                       bool& isCancelRequested,
                       std::list<size_t>& diffIndices)
  : m_DiffIndices(diffIndices),
    m_AIn(aIn),
    m_BIn(bIn),
    m_AOut(aOut),
    m_pBOut(bOut),
    m_IsCancelRequested(isCancelRequested),
    m_Progress(progress),
    m_NumInsertedB(0),
    m_NumDeletedA(0),
    m_NumChanged(0),
    m_Max(m_AIn.getNumLines() + m_BIn.getNumLines() + 1),
    m_pDownVector(2 * m_Max + 2),
    m_pUpVector(2 * m_Max + 2)
{ 
  m_Progress.SetDescription(pProgressDescription);

  //
  // Set-up the progress reporting
  //
  const long NUM_NOTIFICATIONS = 18;
  long numPositions = m_AIn.getNumLines();
  m_NotifyIncrement = numPositions / NUM_NOTIFICATIONS;
  m_PercentIncrement = 90 / NUM_NOTIFICATIONS;
  m_NextNotifyPosition = m_NotifyIncrement;
  m_Percent = 0;

  m_Progress.SetValue(0);

  m_NumInsertedB = 0;
  m_NumDeletedA = 0;
  m_NumChanged = 0;

  //
  // Calculate the Longest common subsequence
  //   While calculating the lcs the deleted lines in left file m_A are
  //   marked with DiffLine::Deleted and the inserted lines in right
  //   file m_b are marked with DiffLine::Added
  lcs(0, m_AIn.getNumLines(), 0, m_BIn.getNumLines());

  if(m_IsCancelRequested)
  {
    throw "User abort";
  }

  // Progress reporting
  m_Progress.SetValue(90);

  //
  // Optimizing the diffed files for better diff-readability
  //
  optimize(m_AOut);
  optimize(m_pBOut);

  // Progress reporting
  m_Progress.SetValue(95);

  //
  // Calculate the target DiffFilePartitions from src diff files.
  // That means inserting of empty lines in one side when in other
  // side are insertions or Deletions.
  //
  createDiffFiles();

  // Progress reporting
  m_Progress.SetValue(100);
}

DiffEngine::~DiffEngine()
{

}


long DiffEngine::getNumDifferences() const
{
  return m_NumChanged + m_NumDeletedA + m_NumInsertedB;
}


long DiffEngine::getNumAdded() const
{
  return m_NumInsertedB;
}


long DiffEngine::getNumChanged() const
{
  return m_NumChanged;
}


long DiffEngine::getNumDeleted() const
{
  return m_NumDeletedA;
}


void DiffEngine::createDiffFiles()
{
  size_t lineA = 0;
  size_t lineB = 0;

  // Clearing these variables as from now on they should not count
  //single lines anymore. Instead they should count difference blocks
  // of according type. They will be set anew below.
  m_NumInsertedB = 0;
  m_NumDeletedA = 0;
  m_NumChanged = 0;

  while (lineA < m_AIn.getNumLines() || lineB < m_BIn.getNumLines())
  {
    //
    // Handle the equal lines
    //
    while( (lineA < m_AIn.getNumLines())
     && (m_AIn[lineA]->getState() == DiffLine::Normal)
     && (lineB < m_BIn.getNumLines())
     && (m_BIn[lineB]->getState() == DiffLine::Normal))
    {
      const DiffLine* pA = m_AIn[lineA++];
      const DiffLine* pB = m_BIn[lineB++];

      m_AOut.addLine(pA->getText(), DiffLine::Normal, pA->getLineNumText());
      m_pBOut.addLine(pB->getText(), DiffLine::Normal, pB->getLineNumText());
    }

    //
    // Handle changed, deleted, inserted lines
    //
    bool bBlockAlreadyAdded = false;
    while((lineA < m_AIn.getNumLines())
      && (lineB < m_BIn.getNumLines())
      && (m_AIn[lineA]->getState() != DiffLine::Normal)
      && (m_BIn[lineB]->getState() != DiffLine::Normal))
    {
      const DiffLine* pA = m_AIn[lineA++];
      const DiffLine* pB = m_BIn[lineB++];

      long idx = m_AOut.addLine(pA->getText(), DiffLine::Changed, pA->getLineNumText());
      m_pBOut.addLine(pB->getText(), DiffLine::Changed, pB->getLineNumText());

      if(!bBlockAlreadyAdded)
      {
        // Add start of this block of CHANGED lines to differences list
        m_NumChanged++;
        bBlockAlreadyAdded = true;
        m_DiffIndices.push_back(idx); // -2 ... has been decremented above
      }
    }

    bBlockAlreadyAdded = false;
    while((lineA < m_AIn.getNumLines())
       && (lineB >= m_BIn.getNumLines() || (m_AIn[lineA]->getState() != DiffLine::Normal)))
    {
      const DiffLine* pA = m_AIn[lineA++];
      long idx = m_AOut.addLine(pA->getText(), DiffLine::Deleted, pA->getLineNumText());
      m_pBOut.addEmptyLine();

      if(!bBlockAlreadyAdded)
      {
        // Add start of this block of DELETED lines to differences list
        m_NumDeletedA++;
        bBlockAlreadyAdded = true;
        m_DiffIndices.push_back(idx);
      }
    }

    bBlockAlreadyAdded = false;
    while((lineB < m_BIn.getNumLines())
      && (lineA >= m_AIn.getNumLines() || (m_BIn[lineB]->getState() != DiffLine::Normal)))
    {
      const DiffLine* pB = m_BIn[lineB++];
      m_AOut.addEmptyLine();
      long idx = m_pBOut.addLine(pB->getText(), DiffLine::Added, pB->getLineNumText());

      if(!bBlockAlreadyAdded)
      {
        // Add start of this block of ADDED lines to differences list
        m_NumInsertedB++;
        bBlockAlreadyAdded = true;
        m_DiffIndices.push_back(idx);
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
    if(m_IsCancelRequested)
    {
      throw "User abort";
    }

    m_Percent += m_PercentIncrement;
    m_NextNotifyPosition += m_NotifyIncrement;
    m_Progress.SetValue(m_Percent);
  }

  // Fast walkthrough equal lines at the start
  while((lowerA < upperA) && (lowerB < upperB)
     && (m_AIn[lowerA]->getToken() == m_BIn[lowerB]->getToken()))
  {
    lowerA++;
    lowerB++;
  }

  // Fast walkthrough equal lines at the end
  while((lowerA < upperA) && (lowerB < upperB)
     && (m_AIn[upperA - 1]->getToken() == m_BIn[upperB - 1]->getToken()))
  {
    --upperA;
    --upperB;
  }

  if(lowerA == upperA)
  {
    while(lowerB < upperB)
    {
      m_BIn[lowerB++]->setState(DiffLine::Added);
      m_NumInsertedB++;
    }
  }
  else if(lowerB == upperB)
  {
    while(lowerA < upperA)
    {
      m_AIn[lowerA++]->setState(DiffLine::Deleted);
      m_NumDeletedA++;
    }
  }
  else
  {
    Pair smsrd = sms(lowerA, upperA, lowerB, upperB);
    lcs(lowerA, smsrd.Left(), lowerB, smsrd.Top());

    if(m_IsCancelRequested)
    {
      throw "User abort";
    }

    lcs(smsrd.Left(), upperA, smsrd.Top(), upperB);

    if(m_IsCancelRequested)
    {
      throw "User abort";
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
          && (m_AIn[x]->getToken() == m_BIn[y]->getToken()))
          //&& (m_A[x]->Text() == m_B[y]->Text()))
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
          && (m_AIn[x - 1]->getToken() == m_BIn[y - 1]->getToken()))
          //&& (m_A[x - 1]->Text() == m_B[y - 1]->Text()))
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


void DiffEngine::optimize(DiffFileBase& diffFile)
{
  size_t dataLength = diffFile.getNumLines();
  size_t startPos = 0;
  size_t endPos = 0;

  while(startPos < diffFile.getNumLines())
  {
    while((startPos < dataLength)
       && (diffFile[startPos]->getState() == DiffLine::Normal))  // normal
    {
      startPos++;
    }

    endPos = startPos;

    while((endPos < dataLength)
      && (diffFile[startPos]->getState() != DiffLine::Normal)) // modified
    {
      endPos++;
    }

    if((endPos < dataLength)
     && (diffFile[startPos]->getToken() == diffFile[endPos]->getToken()))
    {
      diffFile[endPos]->setState(diffFile[startPos]->getState());
      diffFile[startPos]->setState(DiffLine::Normal);
    }
    else
    {
      startPos = endPos;
    }
  }
}
