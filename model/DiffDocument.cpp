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
  return m_DiffFilePartition.NumLines();
}

const SimpleString* DiffDocument::GetFirstLine()
{
  const SimpleString* pLine =  &m_DiffFilePartition.GetFirstDiffLine()->GetText();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentDiffLine()->GetState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetCurrentLine()
{
  const SimpleString* pLine =  &m_DiffFilePartition.GetCurrentDiffLine()->GetText();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentDiffLine()->GetState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetPreviousLine()
{
  const SimpleString* pLine =  &m_DiffFilePartition.GetPreviousDiffLine()->GetText();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentDiffLine()->GetState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetNextLine()
{
  const SimpleString* pLine =  &m_DiffFilePartition.GetNextDiffLine()->GetText();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentDiffLine()->GetState();
  return evaluateLine(pLine, lineState);
}

const SimpleString* DiffDocument::GetIndexedLine(int p_LineIdx)
{
  const SimpleString* pLine =  &m_DiffFilePartition.GetIndexedDiffLine(p_LineIdx)->GetText();
  DiffLine::LineState lineState = m_DiffFilePartition.GetCurrentDiffLine()->GetState();
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
