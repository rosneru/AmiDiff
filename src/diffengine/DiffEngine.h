#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "SimpleString.h"
#include "DiffLine.h"
#include "DiffFilePartition.h"
#include "ProgressReporter.h"

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

  int shortestEdit(DiffFilePartition& a, DiffFilePartition& b);

};

#endif
