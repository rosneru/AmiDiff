#include "DiffDocument.h"

DiffDocument::DiffDocument(DiffFilePartition& p_DiffFilePartition)
  : m_DiffFilePartition(p_DiffFilePartition)
{

}

DiffDocument::~DiffDocument()
{

}

const size_t DiffDocument::NumLines() const
{
  return m_DiffFilePartition.NumberOfLines();
}

const SimpleString* DiffDocument::GetFirstLine()
{
  const SimpleString* pLine =  m_DiffFilePartition.GetFirstRawLine();
  DiffLine::LineState lineState = m_DiffFilePartition.GetFirstLineState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetCurrentLine()
{
  const SimpleString* pLine =  m_DiffFilePartition.GetCurrentRawLine();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentLineState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetPreviousLine()
{
  const SimpleString* pLine =  m_DiffFilePartition.GetPreviousRawLine();
  DiffLine::LineState lineState = m_DiffFilePartition.GetPreviousLineState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetNextLine()
{
  const SimpleString* pLine =  m_DiffFilePartition.GetNextRawLine();
  DiffLine::LineState lineState = m_DiffFilePartition.GetNextLineState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetIndexedLine(int p_LineIdx)
{
  const SimpleString* pLine =  m_DiffFilePartition.GetIndexedRawLine(p_LineIdx);
  DiffLine::LineState lineState = m_DiffFilePartition.GetIndexedLineState(p_LineIdx);
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::evaluateLine(const SimpleString* p_pLine, DiffLine::LineState p_LineState)
{
  if(p_pLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  switch(p_LineState)
  {
    case DiffLine::Normal:
      m_ColorNameBackground = TextDocument::CN_Default;
      break;

    case DiffLine::Changed:
      m_ColorNameBackground = TextDocument::CN_Yellow;
      break;

    case DiffLine::Added:
      m_ColorNameBackground = TextDocument::CN_Green;
      break;

    case DiffLine::Deleted:
      m_ColorNameBackground = TextDocument::CN_Red;
      break;

    case DiffLine::Undefined:
      m_ColorNameBackground = TextDocument::CN_Default;
      break;
  }

  return p_pLine;
}
