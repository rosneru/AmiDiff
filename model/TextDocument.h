#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "Document.h"
#include "LinkedList.h"
#include "SimpleString.h"


class TextDocument : public Document
{
public:
  TextDocument();
  virtual ~TextDocument();
  bool Load(const SimpleString& p_FileName);
  void Clear();



  virtual const size_t NumLines() const;

  virtual const SimpleString* GetFirstLine();
  virtual const SimpleString* GetCurrentLine();
  virtual const SimpleString* GetPreviousLine();
  virtual const SimpleString* GetNextLine();
  virtual const SimpleString* GetIndexedLine(int p_LineIdx);

protected:
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
  LinkedList m_Lines;
};


#endif // TEXT_DOCUMENT_H
