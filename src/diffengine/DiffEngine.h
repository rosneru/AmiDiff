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
 * This is an implementation of the Myers diff algorithm. I ported it
 * from a c# solution I found in the web. Here the attached license:
 *
 *      diff.cs: A port of the algorythm to C#
 *      Copyright (c) by Matthias Hertel, http://www.mathertel.de
 *      This work is licensed under a BSD style license.
 *      See http://www.mathertel.de/License.aspx
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

  /**
   * The divide-and-conquer implementation of the longest-common-
   * subsequence (LCS) algorithm. The published algorithm passes
   * recursively parts of the A and B sequences. To avoid copying
   * these arrays the lower and upper bounds are passed while the
   * sequences stay constant.
   */
  void lcs(long lowerA, long upperA, long lowerB, long upperB);


  /**
   * This is the algorithm to find the Shortest Middle Snake (SMS)
   */
  Pair shortestMiddleSnake(long lowerA,
                           long upperA,
                           long lowerB,
                           long upperB);

  /**
   * If a sequence of modified lines starts with a line that
   * contains the same content as the line that appends the
   * changes, the difference sequence is modified so that the
   * appended line and not the starting line is marked as modified.
   *
   * This leads to more readable diff sequences when comparing
   * text files.
   */
  void optimize(DiffFilePartition& data);

  void reportProgress(int progress);
};

#endif
