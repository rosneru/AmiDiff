#include "DiffEngine.h"



DiffEngine::DiffEngine()
{

}

bool DiffEngine::Diff(DiffFilePartition& p_File1Src,
                      DiffFilePartition& p_File2Src,
                      DiffFilePartition& p_File1Diff,
                      DiffFilePartition& p_File2Diff)
{
  if(p_File1Src.NumberOfLines() == 0)
  {
    long i = 0;
    while(i < p_File2Src.NumberOfLines())
    {
      p_File1Diff.AddBlankLine();
      p_File2Diff.AddString(p_File2Src.GetIndexedRawLine(i++), DiffLine::Normal);
    }

    return true;
  }

  long i = 0;
  long nF2CurrentLine = 0;

  while(i < p_File1Src.NumberOfLines())
  {
    // process this line (and possibly update indexes as well)
    long nLineF2 = nF2CurrentLine;
    if(nLineF2 >= p_File2Src.NumberOfLines())
    {
      DiffLine* pFile1DiffLine = p_File1Src.GetIndexedDiffLine(i);
      while(pFile1DiffLine != NULL)
      {
        // It's time to end the game now
        p_File1Diff.AddString(pFile1DiffLine->GetLine(), DiffLine::Deleted);
        p_File2Diff.AddBlankLine();
        pFile1DiffLine = p_File1Src.GetNextDiffLine();
      }
      break;
    }


    if(p_File1Src.MatchLine(i, p_File2Src, nLineF2))
    {
      bool bDeleted = false;
      if(nLineF2 > nF2CurrentLine)
      {
        long iTmp = i;
        bDeleted = p_File2Src.MatchLine(nF2CurrentLine, p_File1Src, iTmp) && (iTmp  < p_File1Src.NumberOfLines());
        if(bDeleted)
        {
          long j = iTmp - i;
          DiffLine* pFile1DiffLine = p_File1Src.GetIndexedDiffLine(i);
          while(j > 0 && pFile1DiffLine != NULL)
          {
            p_File1Diff.AddString(pFile1DiffLine->GetLine(), DiffLine::Deleted);
            p_File2Diff.AddBlankLine();
            j--;
            pFile1DiffLine = p_File1Src.GetNextDiffLine();
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
         DiffLine* pFile2DiffLine = p_File2Src.GetIndexedDiffLine(nLineF2 - j);
         while (j > 0 && pFile2DiffLine != NULL)
         {
           p_File1Diff.AddBlankLine();
           p_File2Diff.AddString(pFile2DiffLine->GetLine(), DiffLine::Added );

           j--;
           pFile2DiffLine = p_File2Src.GetNextDiffLine();
         }
      }

      // exactly matched
      p_File1Diff.AddString(p_File1Src.GetIndexedRawLine(i), DiffLine::Normal);
      p_File2Diff.AddString(p_File2Src.GetIndexedRawLine(nLineF2), DiffLine::Normal);

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
        p_File1Diff.AddString( p_File1Src.GetIndexedRawLine(i), DiffLine::Deleted);
        p_File2Diff.AddBlankLine();

        // this whole block is flagged as deleted
        if (nLineF1 > (i + 1))
        {
          long j = nLineF1 - (i + 1);
          DiffLine* pFile1DiffLine = p_File1Src.GetIndexedDiffLine(i + 1);
          while (j > 0 && pFile1DiffLine != NULL)
          {
            i++;

            p_File1Diff.AddString( pFile1DiffLine->GetLine(), DiffLine::Deleted);
            p_File2Diff.AddBlankLine();

            pFile1DiffLine = p_File1Src.GetNextDiffLine();
            j--;
          }
        }

        // note : nF2CurrentLine is not incremented
      }
      else
      {
        // neither added, nor deleted, so it's flagged as changed
        p_File1Diff.AddString(p_File1Src.GetIndexedRawLine(i), DiffLine::Changed);
        p_File2Diff.AddString(p_File2Src.GetIndexedRawLine(nLineF2), DiffLine::Changed);

        nF2CurrentLine = nLineF2 + 1; // next line in p_File2Src
      }
    }

    i++; // next line in p_File1Src
  }

  // are there any remaining lines from p_File2Src?
  DiffLine* pFile2DiffLine = p_File2Src.GetIndexedDiffLine(nF2CurrentLine);
  while (nF2CurrentLine < p_File2Src.NumberOfLines() && pFile2DiffLine != NULL)
  {
    p_File1Diff.AddBlankLine();
    p_File2Diff.AddString(pFile2DiffLine->GetLine(), DiffLine::Added );

    pFile2DiffLine = p_File2Diff.GetNextDiffLine();
    nF2CurrentLine++;
  }

  return true;
}
