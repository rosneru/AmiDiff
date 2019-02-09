#include "DiffEngine.h"

DiffEngine::DiffEngine(bool& p_bCancelRequested)
  : m_bCancelRequested(p_bCancelRequested),
    m_pProgressReporter(NULL)
{

}

bool DiffEngine::Diff(DiffFilePartition& p_File1Src,
                      DiffFilePartition& p_File2Src,
                      DiffFilePartition& p_File1Diff,
                      DiffFilePartition& p_File2Diff)
{
  long numLinesFile1Src = p_File1Src.NumLines();
  long numLinesFile2Src = p_File2Src.NumLines();
  int lastProgressValue = -1;

  // special empty file case
  if(numLinesFile1Src == 0)
  {
    //
    // Progress reporting (initial value)
    //
    if(m_pProgressReporter != NULL)
    {
      m_pProgressReporter->notifyProgressChanged(10);
    }

    long i = 0;
    while(i < numLinesFile2Src)
    {
      p_File1Diff.AddBlankLine();
      p_File2Diff.AddString(p_File2Src.GetDiffLineText(i++), DiffLine::Normal);
    }

    //
    // Progress reporting (final value)
    //
    if(m_pProgressReporter != NULL)
    {
      m_pProgressReporter->notifyProgressChanged(100);
    }

    return true;
  }

  long i = 0;
  long nF2CurrentLine = 0;

  while(i < numLinesFile1Src)
  {
    // process this line (and possibly update indexes as well)
    long nLineF2 = nF2CurrentLine;
    if(nLineF2 >= numLinesFile2Src)
    {
      while(i < numLinesFile1Src)
      {
        // It's time to end the game now
        p_File1Diff.AddString(p_File1Src.GetDiffLine(i)->Text(), DiffLine::Deleted);
        p_File2Diff.AddBlankLine();
        i++;
      }
      break;
    }


    if(p_File1Src.MatchLine(i, p_File2Src, nLineF2))
    {
      bool bDeleted = false;
      if(nLineF2 > nF2CurrentLine)
      {
        long iTmp = i;
        bDeleted = p_File2Src.MatchLine(nF2CurrentLine, p_File1Src, iTmp) && (iTmp  < numLinesFile1Src);
        if(bDeleted)
        {
          long j = iTmp - 1;
          while(j > 0)
          {
            p_File1Diff.AddString(p_File1Src.GetDiffLine(i)->Text(), DiffLine::Deleted);
            p_File2Diff.AddBlankLine();
            i++;
            j--;
          }

          // please note nf2CurrentLine is not updated

          continue; // jump here to loop iteration
        }
      }

      // matched, so either the lines were identical, or p_File2Src
      // has added one or more lines
      if (nLineF2 > nF2CurrentLine)
      {
         // add blank lines to p_File1Diff
         long j = nLineF2 - nF2CurrentLine;
         while (j > 0)
         {
           p_File1Diff.AddBlankLine();
           p_File2Diff.AddString(p_File2Src.GetDiffLine(nLineF2 - j)->Text(), DiffLine::Added );

           j--;
         }
      }

      // exactly matched
      p_File1Diff.AddString(p_File1Src.GetDiffLineText(i), DiffLine::Normal);
      p_File2Diff.AddString(p_File2Src.GetDiffLineText(nLineF2), DiffLine::Normal);

      nF2CurrentLine = nLineF2 + 1; // next line in p_File2Src
    }
    else
    {
      // This line is not found at all in f2, either it's because it
      // has been changed, or even deleted
      long nLineF1 = i;

      if (p_File2Src.MatchLine(nLineF2, p_File1Src, nLineF1) )
      {
        // the dual line in f2 can be found in f1, that's because
        // the current line in f1 has been deleted
        p_File1Diff.AddString( p_File1Src.GetDiffLineText(i), DiffLine::Deleted);
        p_File2Diff.AddBlankLine();

        // this whole block is flagged as deleted
        if (nLineF1 > (i + 1))
        {
          long j = nLineF1 - (i + 1);
          while (j > 0)
          {
            i++;

            p_File1Diff.AddString(p_File1Src.GetDiffLine(i)->Text(), DiffLine::Deleted);
            p_File2Diff.AddBlankLine();

            j--;
          }
        }

        // note : nF2CurrentLine is not incremented
      }
      else
      {
        // neither added, nor deleted, so it's flagged as changed
        p_File1Diff.AddString(p_File1Src.GetDiffLineText(i), DiffLine::Changed);
        p_File2Diff.AddString(p_File2Src.GetDiffLineText(nLineF2), DiffLine::Changed);

        nF2CurrentLine = nLineF2 + 1; // next line in p_File2Src
      }
    }

    i++; // next line in p_File1Src

    //
    // Progress reporting
    //
    if(m_pProgressReporter != NULL)
    {
      // Report the 'lastProgressValue - 1' to ensure that the final
      // value of 100 (%) is sent after the last line is read.
      int newProgressValue = (i * 100 / numLinesFile1Src) - 1;

      if(newProgressValue > lastProgressValue)
      {
       // For performance reasons report only 3% steps
       if(newProgressValue % 3 == 0)
        {
          lastProgressValue = newProgressValue;
          m_pProgressReporter->notifyProgressChanged(lastProgressValue);
        }
      }
    }

    //
    // Handling a potential cancel request
    //
    if(m_bCancelRequested == true)
    {
      m_bCancelRequested = false;
      return false;
    }


  }

  // are there any remaining lines from p_File2Src?
  while (nF2CurrentLine < numLinesFile2Src)
  {
    p_File1Diff.AddBlankLine();
    p_File2Diff.AddString(p_File2Src.GetDiffLine(nF2CurrentLine)->Text(), DiffLine::Added );

    nF2CurrentLine++;
  }

  //
  // Progress reporting (final value)
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
