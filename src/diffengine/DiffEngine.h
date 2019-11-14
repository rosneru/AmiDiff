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
  DiffEngine(bool& bCancelRequested);
  ~DiffEngine();

  bool Diff(DiffFilePartition& srcA,
            DiffFilePartition& srcB,
            DiffFilePartition& diffA,
            DiffFilePartition& diffB);


  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;  ///> for progress reporting
  long m_Max;

  Pair shortestMiddleSnake(DiffFilePartition& a,
                           long lowerA,
                           long upperA,
                           DiffFilePartition& b,
                           long lowerB,
                           long upperB,
                           long* pDownVector,
                           long* pUpVector);

  void lcs(DiffFilePartition& a,
           long lowerA,
           long upperA,
           DiffFilePartition& b,
           long lowerB,
           long upperB,
           long* pDownVector,
           long* pUpVector);
};

#endif
