#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "SimpleString.h"


class Document
{
public:
  Document();
  virtual ~Document();

  // TODO Fix this
  // virtual bool Load(const char* pFileName) = 0;
  // virtual void Clear() = 0;

  const char*FileName() const;
  void SetFileName(const char* pFileName);

  /**
   * Used to describe if the last line obtaining method was
   * getNextLine(), getPreviousLine() ore none of both.
   */
  enum LastScrollDirection
  {
    None,
    NextLine,
    PreviousLine,
  };


protected:
  char* m_FileName;
  size_t m_MaxLineLength;

  /**
   * Stores if the last scroll line obtaining method was getNextLine(),
   * getPreviousLine() ore none of both.
   */
  LastScrollDirection m_LastScrollDirection;
};


#endif // DOCUMENT_H
