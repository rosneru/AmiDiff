#ifndef DIFF_INPUT_FILE_BASE_H
#define DIFF_INPUT_FILE_BASE_H

#include "DiffFileBase.h"

class DiffInputFileBase : public DiffFileBase
{
public:
  DiffInputFileBase(bool& bCancelRequested);
  virtual ~DiffInputFileBase();

protected:
  bool& m_bCancelRequested;

  /**
   * Returns the number of digits of a given positive number.
   */
  size_t numDigits(size_t number);
};

#endif
