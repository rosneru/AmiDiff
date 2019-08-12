#ifndef DIFF_TRACER_H
#define DIFF_TRACER_H

#include "Array.h"
#include "LinkedList.h"

#include "DiffFilePartition.h"

/**
 * A class that can store multiple arrays in a list for doing a trace 
 * later
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

  void AddTrace(const int* array, size_t arrayLen);

  Array<int>* GetFirst();
  Array<int>* GetNext();
  size_t NumItems();

  void Backtrack(int x, int y);

protected:
  DiffFilePartition& m_A;
  DiffFilePartition& m_B;
  LinkedList m_TraceList;
};

#endif
