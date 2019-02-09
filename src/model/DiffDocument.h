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

  size_t NumLines() const;
  size_t MaxLineLength();

  const SimpleString* GetIndexedLine(int p_LineId);

  ColorName LineColor() const;



private:
  DiffFilePartition& m_DiffFilePartition;
  size_t m_LineId;
  ColorName m_LineColor;
  const SimpleString* evaluateLine(const DiffLine* p_pDiffLine);
};

#endif // DIFF_DOCUMENT_H
