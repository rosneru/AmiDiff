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
  virtual bool Load(const SimpleString& p_FileName);
  void Clear();

  const size_t NumLines() const;

  const SimpleString* GetFirstLine();
  const SimpleString* GetCurrentLine();
  const SimpleString* GetPreviousLine();
  const SimpleString* GetNextLine();
  const SimpleString* GetIndexedLine(int p_LineIdx);

private:
  LinkedList m_Lines;
};


#endif // TEXT_DOCUMENT_H
