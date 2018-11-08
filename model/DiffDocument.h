#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffFilePartition.h"
#include "DiffLine.h"
#include "SimpleString.h"
#include "TextDocument.h"

class DiffDocument : public TextDocument
{
public:
  DiffDocument(DiffFilePartition& p_DiffFilePartition);
  virtual ~DiffDocument();

  virtual const size_t NumLines() const;

  const SimpleString* GetFirstLine();
  const SimpleString* GetCurrentLine();
  const SimpleString* GetPreviousLine();
  const SimpleString* GetNextLine();
  const SimpleString* GetIndexedLine(int p_LineIdx);

private:
  DiffFilePartition& m_DiffFilePartition;

  const SimpleString* evaluateLine(const SimpleString* p_pLine, DiffLine::LineState p_LineState);
};

#endif // DIFF_DOCUMENT_H
