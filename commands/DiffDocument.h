#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "SimpleString.h"

class DiffDocument
{
public:
  virtual ~DiffDocument();

  virtual bool Open() = 0;

protected:
  DiffDocument();
};

#endif // DIFF_DOCUMENT_H
