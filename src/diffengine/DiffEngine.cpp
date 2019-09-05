#include "DiffEngine.h"

DiffEngine::DiffEngine(bool& bCancelRequested)
  : m_bCancelRequested(bCancelRequested),
    m_pProgressReporter(NULL)
{

}

bool DiffEngine::Diff(DiffFilePartition& srcA,
                      DiffFilePartition& srcB,
                      DiffFilePartition& targetA,
                      DiffFilePartition& targetB)
{
  DiffTrace trace(srcA, srcB, targetA, targetB);
  if(shortestEdit(trace, srcA, srcB) == false)
  {
    return false;
  }

  trace.Backtrack();

//  targetA.SetReversedMode(true);
//  targetB.SetReversedMode(true);

  return true;
}



void DiffEngine::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}

/**
 * @brief
 * Convert the given index into a 'Ruby-like-array-index' in regards to
 * the target-array's dimension (from- and to-value).
 *
 * That means the target arrays index will not be exceeded or deceeded.
 * When due to exceeding the remaining 'id-portion' is added from the
 * start, deceeding it's subtracted from the end.
 *
 * TODO This is ugly by some means. Find a better solution!
 */
int idxConvert(int i, int from, int to)
{
  if(i < 0)  // use 'from' instead of '0' ??
  {
    return from + to + i;
  }

  if(i > to)
  {
    return from + (i - to);
  }

  return i;
}


bool DiffEngine::shortestEdit(DiffTrace& trace,
                              DiffFilePartition& a,
                              DiffFilePartition& b)
{
  int n = a.NumLines();
  int m = b.NumLines();
  long max = n + m;

  int vSize = 2 * max + 1;
  int* v = new int[vSize];
  v[1] = 0;

  int x, y;

  for(int d = 0; d <= max; d++)
  {
    trace.AddTrace(v, vSize);
    for(long k = -d; k <= d; k += 2)
    {
      if((k == -d) || ((k != d) && (v[idxConvert(k - 1, 1, vSize)] < v[idxConvert(k + 1, 1, vSize)])))
      {
        x = v[idxConvert(k + 1, 1, vSize)];
      }
      else
      {
        x = v[idxConvert(k - 1, 1, vSize)] + 1;
      }

      y = x - k;

      while((x < n) && (y < m) &&
            (a.GetDiffLine(x)->Token() == b.GetDiffLine(y)->Token()))
      {
        if(a.GetDiffLine(x)->Text() == b.GetDiffLine(y)->Text())
        {
          x++;
          y++;
        }
      }

      int idx = idxConvert(k, 1, vSize);

      v[idx] = x;

      if((x >= n) && (y >= m))
      {
        delete[] v;
        return true;
      }
    }
  }

  delete[] v;
  return false;
}
