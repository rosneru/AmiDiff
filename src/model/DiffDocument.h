#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffFilePartitionAmiga.h"
#include "DiffLine.h"
#include "SimpleString.h"
#include "Document.h"

class DiffDocument : public Document
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

  DiffDocument(DiffFilePartitionAmiga& p_DiffFilePartition);
  virtual ~DiffDocument();

  size_t NumLines() const;
  size_t MaxLineLength();

  const DiffLine* GetIndexedLine(int p_LineId);

  ColorName LineColor() const;

private:
  DiffFilePartitionAmiga& m_DiffFilePartition;
  size_t m_LineId;
  ColorName m_LineColor;
  const DiffLine* evaluateLine(const DiffLine* pDiffLine);
};

#endif // DIFF_DOCUMENT_H
