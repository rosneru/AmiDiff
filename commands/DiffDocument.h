#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "SimpleString.h"

class DiffDocument
{
public:
  DiffDocument(SimpleString p_Name);
  virtual ~DiffDocument();

  bool Open(SimpleString p_FileName);
  SimpleString Name();

private:
  SimpleString m_Name;
};

#endif // DIFF_DOCUMENT_H
