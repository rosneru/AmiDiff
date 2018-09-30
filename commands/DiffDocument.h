#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "LinkedList.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  virtual ~DiffDocument();

  void Clear();
  virtual bool ReadFile(SimpleString p_FileName = "");

  SimpleString* GetFirstLine();
  SimpleString* GetCurrentLine();
  SimpleString* GetPreviousLine();
  SimpleString* GetNextLine();
  SimpleString* GetIndexedLine(int p_lineIdx);

protected:
  DiffDocument();
  const size_t MAX_LINE_LENGTH;

private:
  LinkedList* m_pLines; ///> Pointer to a list containig the lines
};

#endif // DIFF_DOCUMENT_H
