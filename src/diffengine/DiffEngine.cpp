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
  int se = shortestEdit(srcA, srcB);

  return se > 0;
}



void DiffEngine::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}

int DiffEngine::shortestEdit(DiffFilePartition& a, DiffFilePartition& b)
{
  int n = a.NumLines();
  int m = b.NumLines();
  long max = n + m;

  int vSize = 2 * max + 1;
  int* v = new int[vSize];
  v[1] = 0;

  int x, y;

  for(int d = 0; d < max; d++)
  {
    for(long k = -d; k <= d; k += 2)
    {
      if((k == -d) || ((k != d) && (v[k - 1] < v[k + 1])))
      {
        x = v[k + 1];
      }
      else
      {
        x = v[k - 1] + 1;
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

      int idx = k;
      if(idx < 0)
      {
        idx = vSize + k;
      }

      v[idx] = x;

      if((x >= n) && (y >= m))
      {
        delete[] v;
        return d;
      }
    }
  }

  delete[] v;
  return -1;
}
