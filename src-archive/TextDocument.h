#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "Document.h"
#include <vector>
#include "SimpleString.h"


class TextDocument : public Document
{
public:
  TextDocument();
  virtual ~TextDocument();
  bool Load(const SimpleString& p_FileName);
  void Clear();

  virtual size_t NumLines() const;
  virtual size_t MaxLineLength();
  virtual const SimpleString* GetIndexedLine(int p_LineIdx);

protected:
  size_t m_MaxLineLength;

private:
  Array<SimpleString*> m_Lines;
};


#endif // TEXT_DOCUMENT_H
