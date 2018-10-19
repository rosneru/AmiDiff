#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "LinkedList.h"
#include "SimpleString.h"


class TextDocument
{
public:
  TextDocument();
  ~TextDocument();
  bool Load(SimpleString p_FileName);
  void Clear();

  SimpleString* GetFirstLine();
  SimpleString* GetCurrentLine();
  SimpleString* GetPreviousLine();
  SimpleString* GetNextLine();
  size_t NumLines();

private:
  LinkedList* m_pLines;
  SimpleString* m_pCurrentLine;
};


#endif // TEXT_DOCUMENT_H
