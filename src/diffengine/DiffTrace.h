#ifndef DIFF_TRACER_H
#define DIFF_TRACER_H

#include "Array.h"
#include "LinkedList.h"

#include "DiffFilePartition.h"

/**
 * A class that can store multiple arrays in a list for doing a trace 
 * later.
 *
 * After the Backtrack() method is called the target references given
 * in the constructor will contain files with diff informations ready
 * to be displayed.
 *
 * @author Uwe Rosner
 * @date 26/07/2019
 */
class DiffTrace
{
public:
  DiffTrace(DiffFilePartition& srcA,
            DiffFilePartition& srcB,
            DiffFilePartition& targetA,
            DiffFilePartition& targetB);

  ~DiffTrace();

  /**
   * @brief
   * Add an array to the TraceList.
   */
  void AddTrace(const int* array, size_t arrayLen);


  /**
   * @brief
   * Backtrack all trace lists to find the single path which is leading
   * to the result.
   */
  void Backtrack();

  /**
   * @brief
   * Helper method!
   *
   * Convert the given index into a 'Ruby-like-array-index' in regards to
   * the target-array's dimension (from- and to-value).
   *
   * That means the target arrays index will not be exceeded or deceeded.
   * When due to exceeding the remaining 'id-portion' is added from the
   * start, deceeding it's subtracted from the end.
   */
  static size_t IdxConv(int idx, int arraySize);

private:
  DiffFilePartition& m_SrcA;
  DiffFilePartition& m_SrcB;
  DiffFilePartition& m_TargetA;
  DiffFilePartition& m_TargetB;
  LinkedList m_TraceList;

  void yield(int x1, int y1, int x, int y);

  /**
   * @brief
   * Returns the first Array which is stored in the TraceList.
   */
  Array<int>* firstTrace();

  /**
   * @brief
   * Returns the following Array from the TraceList.
   */
  Array<int>* nextTrace();

  /**
   * @brief
   * Returns the number of Arrays which are stored in the TraceList.
   */
  size_t numTraces();
};

#endif
