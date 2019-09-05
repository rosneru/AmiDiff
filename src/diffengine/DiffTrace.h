#ifndef DIFF_TRACER_H
#define DIFF_TRACER_H

#include "Array.h"
#include "LinkedList.h"

#include "DiffFilePartition.h"

/**
 * A class that can store multiple arrays in a list for doing a trace 
 * later.
 *
 *
 * @author Uwe Rosner
 * @date 26/07/2019
 */
class DiffTrace
{
public:
  DiffTrace(DiffFilePartition& a, DiffFilePartition& b);

  ~DiffTrace();

  /**
   * @brief
   * Add an array to the TraceList.
   */
  void AddTrace(const int* array, size_t arrayLen);

  /**
   * @brief
   * Returns the first Array which is stored in the TraceList.
   */
  Array<int>* GetFirst();

  /**
   * @brief
   * Returns the following Array from the TraceList.
   */
  Array<int>* GetNext();

  /**
   * @brief
   * Returns the number of Arrays which are stored in the TraceList.
   */
  size_t NumItems();

  /**
   * @brief
   * Backtrack all trace lists to find the single path which is leading
   * to the result.
   */
  void Backtrack();

  /**
   * @brief GetDiffResult
   * Fill the given target DiffFilePartitions with the calculated Diff
   */
  bool GetDiffResult(DiffFilePartition& targetA,
                     DiffFilePartition& targetB);

protected:
  DiffFilePartition& m_A;
  DiffFilePartition& m_B;
  LinkedList m_TraceList;

private:
  void yield(int x1, int y1, int x, int y);
};

#endif
