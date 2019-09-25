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
  Trace trace(srcA, srcB, targetA, targetB);
  if(shortestEdit(trace, srcA, srcB) == false)
  {
    return false;
  }

  //
  // Progress reporting
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(50);
  }

  trace.Backtrack();

  targetA.SetReversedMode(true);
  targetB.SetReversedMode(true);

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


bool DiffEngine::shortestEdit(Trace& trace,
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
    //trace.AddTrace(v, vSize);
    for(long k = -d; k <= d; k += 2)
    {
      if((k == -d) || ((k != d) && (v[Trace::IdxConv(k - 1, vSize)] < v[Trace::IdxConv(k + 1, vSize)])))
      {
        x = v[Trace::IdxConv(k + 1, vSize)];
      }
      else
      {
        x = v[Trace::IdxConv(k - 1, vSize)] + 1;
      }

      y = x - k;

      while((x < n) && (y < m) 
        && (a.GetDiffLine(x)->Token() == b.GetDiffLine(y)->Token())
        && (a.GetDiffLine(x)->Text() == b.GetDiffLine(y)->Text()))
      {
        x++;
        y++;
      }

      int idx = Trace::IdxConv(k, vSize);

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

bool DiffEngine::findPath(long left, long top, long right, long bottom)
{
  Box box(left, top, right, bottom);
  Box snake = midpoint(box);
}

Box DiffEngine::midpoint(Box box)
{
  if(box.Size() == 0)
  {
    // If this box is empty return the mepty box to signal the failure
    return box;
  }

  // Original: max = (box.size / 2.0).ceil
  int max = (box.Size() / 2) + ((box.Size() % 2) != 0);

  int vSize = 2 * max + 1;

  int* vf = new int[vSize];
  vf[1] = box.Left();

  int* vb = new int[vSize];
  vb[1] = box.Bottom();

  for(int d = 0; d <= max; d++)
  {

  }
}
