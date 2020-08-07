#ifndef DIFF_INPUT_FILE_BASE_H
#define DIFF_INPUT_FILE_BASE_H

#include "DiffFileBase.h"

class DiffInputFileBase : public DiffFileBase
{
public:
  DiffInputFileBase(bool& isCancelRequested);
  virtual ~DiffInputFileBase();

protected:
  bool& m_IsCancelRequested;

};

#endif
