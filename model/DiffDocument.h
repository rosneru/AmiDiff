#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include <stdlib.h>

#include "LinkedList.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  virtual ~DiffDocument();
  virtual bool ReadFile(SimpleString p_FileName = "");

protected:
  DiffDocument();
  void Clear();
  size_t NumLines();
  SimpleString* GetFirstLine();
  SimpleString* GetCurrentLine();
  SimpleString* GetPreviousLine();
  SimpleString* GetNextLine();
  SimpleString* GetIndexedLine(int p_lineIdx);

  const size_t MAX_LINE_LENGTH;

private:
  SimpleString m_TimeStatistics;
  LinkedList* m_pLines; ///> Pointer to a list containing the lines
};

#endif // DIFF_DOCUMENT_H
