#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "Document.h"
#include "LinkedList.h"
#include "SimpleString.h"


class TextDocument : public Document
{
public:
  TextDocument();
  ~TextDocument();
  bool Load(SimpleString p_FileName);
  void Clear();

  const size_t NumLines();

  SimpleString* GetFirstLine();
  SimpleString* GetCurrentLine();
  SimpleString* GetPreviousLine();
  SimpleString* GetNextLine();
  SimpleString* GetIndexedLine(int p_LineIdx);

private:
  LinkedList m_Lines;
};


#endif // TEXT_DOCUMENT_H
