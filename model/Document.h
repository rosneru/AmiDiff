#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "SimpleString.h"


class Document
{
public:
  Document();
  virtual ~Document();
  virtual bool Load(const SimpleString& p_FileName) = 0;
  virtual void Clear() = 0;

  const SimpleString& FileName();
  const SimpleString& TimeStatistics();

protected:
  SimpleString m_FileName;
  SimpleString m_TimeStatistics;
};


#endif // DOCUMENT_H
