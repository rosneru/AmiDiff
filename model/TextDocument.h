#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "Document.h"
#include "Array.h"
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

  /**
   * Stores if the last scroll line obtaining method was getNextLine(),
   * getPreviousLine() ore none of both.
   */
  LastScrollDirection m_LastScrollDirection;

private:
  Array<SimpleString*> m_Lines;
};


#endif // TEXT_DOCUMENT_H
