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
  long aMax = p_File1Src.NumLines();
  long bMax = p_File2Src.NumLines();

  V.Set(1, 0);

  for(long d = 0; d < (aMax + bMax); d++)
  {
    for(long k = -d; k <= d; k += 2 )
    {
      // down or right?
      bool down = ( k == -d || ( k != d && V[ k - 1 ] < V[ k + 1 ] ) );

      long kPrev = down ? k + 1 : k - 1;

      // start point
      long xStart = V[kPrev];
      long yStart = xStart - kPrev;

      // mid point
      long xMid = down ? xStart : xStart + 1;
      long yMid = xMid - k;

      // end point
      long xEnd = xMid;
      long yEnd = yMid;

      // follow diagonal
      long snake = 0;
      while (    (xEnd < aMax )
              && (yEnd < bMax)
              && (p_File1Src.GetDiffLine(xEnd)->Token() == p_File2Src.GetDiffLine(yEnd)->Token()) )
      {
        xEnd++;
        yEnd++;
        snake++;
        p_File1Diff.AddString(p_File1Src.GetDiffLine(xEnd)->Text(), DiffLine::Normal);
        p_File2Diff.AddString(p_File1Src.GetDiffLine(yEnd)->Text(), DiffLine::Normal);
      }

      // save end point
//      V.Set(k, xEnd);

      // check for solution
      if ( xEnd >= aMax && yEnd >= bMax ) /* solution has been found */
      {
        // TODO return history
        return true;
      }
    }
  }

  return false;
}

void DiffEngine::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}
