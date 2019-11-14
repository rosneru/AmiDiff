#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "LinkedList.h"
#include "SimpleString.h"

#include "Box.h"
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


  void FindPath(DiffFilePartition& a,
                       long left,
                       long top,
                       DiffFilePartition& b,
                       long right,
                       long bottom,
                       int* pDownVector,
                       int* pUpVector);

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;  ///> for progress reporting
  long m_Max;

  Pair shortestMiddleSnake(DiffFilePartition& a, long lowerA, long upperA,
                           DiffFilePartition& b, long lowerB, long upperB,
                           int* pDownVector, int* pUpVector);

  bool midPair(Box& box, DiffFilePartition& a, DiffFilePartition& b);

  bool forwards(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b);
  bool backward(Box& box, int* vf, int* vb, int vSize, int d, DiffFilePartition& a, DiffFilePartition& b);

  void buildDiff(int x1, int y1, int x2, int y2,
                 DiffFilePartition& srcA,
                 DiffFilePartition& srcB,
                 DiffFilePartition& diffA,
                 DiffFilePartition& diffB);

};

#endif
