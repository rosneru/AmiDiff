#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "Pair.h"
#include "DiffLine.h"
#include "DiffFileBase.h"

#include "ProgressReporter.h"

/**
 * Class for generating and representing the difference of two files.
 *
 * This is an implementation of the Myers diff algorithm. I ported it
 * from a c# solution I found in the web. Here the attached license of
 * the source:
 *
 *      Copyright (c) 2005-2012 by Matthias Hertel, http://www.mathertel.de/
 *      All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or
 *      without modification, are permitted provided that the following
 *      conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 *      - Neither the name of the copyright owners nor the names of its
 *        contributors may be used to endorse or promote products
 *        derived from this software without specific prior written
 *        permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Uwe Rosner
 * @date 18/07/2019
 */
class DiffEngine
{

public:
  DiffEngine(DiffFileBase& a,
             DiffFileBase& b,
             DiffFileBase& aDiff,
             DiffFileBase& bDiff,
             bool& bCancelRequested);

  ~DiffEngine();

  bool Diff();


  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  DiffFileBase& m_A;
  DiffFileBase& m_B;
  DiffFileBase& m_ADiff;
  DiffFileBase& m_BDiff;
  bool& m_bCancelRequested;

  ProgressReporter* m_pProgressReporter;

  long m_Max;
  long* m_pDownVector;
  long* m_pUpVector;

  // The next five fields are for progress notification
  int m_Percent;
  int m_PercentIncrement;
  int m_NotifyIncrement;
  long m_CurrentPosition;
  long m_NextNotifyPosition;


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
  void optimize(DiffFileBase& data);

  void reportProgress(int progress);
};

#endif
