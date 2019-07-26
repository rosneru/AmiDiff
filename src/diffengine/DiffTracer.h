#ifndef DIFF_TRACER_H
#define DIFF_TRACER_H

#include "Array.h"
#include "LinkedList.h"

/**
 * A class that can store multiple arrays in a list for doing a trace 
 * later
 *
 *
 * @author Uwe Rosner
 * @date 26/07/2019
 */
class DiffTracer
{
public:
  DiffTracer();
  ~DiffTracer();

  void AddArray(const int* array, size_t arrayLen);

  Array<int>* GetFirst();
  Array<int>* GetNext();

protected:
  LinkedList m_TraceList;
};

#endif
