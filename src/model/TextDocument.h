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
  bool Load(const char* pFileName);
  void Clear();

  virtual size_t NumLines() const;
  virtual size_t MaxLineLength();
  virtual const char* GetIndexedLine(int idx);

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
  Array<const char*> m_Lines;
};


#endif // TEXT_DOCUMENT_H
