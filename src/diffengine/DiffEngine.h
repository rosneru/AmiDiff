#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "Box.h"
#include "SimpleString.h"
#include "DiffLine.h"
#include "DiffFilePartition.h"
#include "ProgressReporter.h"
#include "Trace.h"

/**
 * Class for generating and representing the difference of two files.
 *
 * This is an implementation of the myers diff algorithm.
 *
 * @author Uwe Rosner
 * @date 18/07/2019
 */
class DiffEngine
{

public:
  DiffEngine(bool& bCancelRequested);
  ~DiffEngine();

  bool Diff(DiffFilePartition& srcA,
            DiffFilePartition& srcB,
            DiffFilePartition& targetA,
            DiffFilePartition& targetB);

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;  ///> for progress reporting

  int* vf;
  int* vb;
  int vSize;

  bool shortestEdit(Trace& trace,
                    DiffFilePartition& a,
                    DiffFilePartition& b);

  Box findPath(long left, long top, long right, long bottom, DiffFilePartition& a, DiffFilePartition& b);

  Box midpoint(Box box, DiffFilePartition& a, DiffFilePartition& b);

  Box forwards(Box box, int d, DiffFilePartition& a, DiffFilePartition& b);
  Box backward(Box box, int d, DiffFilePartition& a, DiffFilePartition& b);

};

#endif
