#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "LinkedList.h"
#include "SimpleString.h"

#include "Pair.h"
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
  DiffEngine(DiffFilePartition& a,
             DiffFilePartition& b,
             DiffFilePartition& aDiff,
             DiffFilePartition& bDiff,
             bool& bCancelRequested);

  ~DiffEngine();

  bool Diff();


  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  DiffFilePartition& m_A;
  DiffFilePartition& m_B;
  DiffFilePartition& m_ADiff;
  DiffFilePartition& m_BDiff;
  bool& m_bCancelRequested;

  ProgressReporter* m_pProgressReporter;

  long m_Max;
  long* m_pDownVector;
  long* m_pUpVector;

  Pair shortestMiddleSnake(long lowerA,
                           long upperA,
                           long lowerB,
                           long upperB);

  void lcs(long lowerA, long upperA, long lowerB, long upperB);
};

#endif
