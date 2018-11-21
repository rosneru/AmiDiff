#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffFilePartition.h"
#include "DiffLine.h"
#include "SimpleString.h"
#include "TextDocument.h"

class DiffDocument : public TextDocument
{
public:
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

  DiffDocument(DiffFilePartition& p_DiffFilePartition);
  virtual ~DiffDocument();

  const size_t NumLines() const;

  const SimpleString* GetFirstLine();
  const SimpleString* GetCurrentLine();
  const SimpleString* GetPreviousLine();
  const SimpleString* GetNextLine();
  const SimpleString* GetIndexedLine(int p_LineIdx);

  ColorName LineColor() const;



private:
  ColorName m_LineColor;

  DiffFilePartition& m_DiffFilePartition;

  const SimpleString* evaluateLine(const DiffLine* p_pDiffLine);
};

#endif // DIFF_DOCUMENT_H
