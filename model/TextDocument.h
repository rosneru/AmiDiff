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

  /**
   * Defines the text color names used to set foreground and 
   * background pens for text rendering
   */
  enum ColorName
  {
    CN_Default,
    CN_Red,
    CN_Green,
    CN_Yellow
  };

  virtual const size_t NumLines() const;

  virtual const SimpleString* GetFirstLine();
  virtual const SimpleString* GetCurrentLine();
  virtual const SimpleString* GetPreviousLine();
  virtual const SimpleString* GetNextLine();
  virtual const SimpleString* GetIndexedLine(int p_LineIdx);

  ColorName ColorNameForeground();
  ColorName ColorNameBackground();



protected:
  ColorName m_ColorNameForeground;
  ColorName m_ColorNameBackground;

private:
  LinkedList m_Lines;
};


#endif // TEXT_DOCUMENT_H
