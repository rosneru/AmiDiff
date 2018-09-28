#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "SimpleString.h"

class DiffDocument
{
public:
  virtual ~DiffDocument();

  virtual bool Open(SimpleString p_FileName = "") = 0;

protected:
  DiffDocument();
  const size_t DIFF_MAX_LINE_LENGTH;
};

#endif // DIFF_DOCUMENT_H
