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
  DiffLine* pDiffLine = m_DiffFilePartition.GetFirstDiffLine();
  if(pDiffLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);

}

const SimpleString* DiffDocument::GetCurrentLine()
{
  DiffLine* pDiffLine = m_DiffFilePartition.GetCurrentDiffLine();
  if(pDiffLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);

}

const SimpleString* DiffDocument::GetPreviousLine()
{
  DiffLine* pDiffLine = m_DiffFilePartition.GetPreviousDiffLine();
  if(pDiffLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);

}

const SimpleString* DiffDocument::GetNextLine()
{
  DiffLine* pDiffLine = m_DiffFilePartition.GetNextDiffLine();
  if(pDiffLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);

}

const SimpleString* DiffDocument::GetIndexedLine(int p_LineIdx)
{
  DiffLine* pDiffLine = m_DiffFilePartition.GetIndexedDiffLine(p_LineIdx);
  if(pDiffLine == NULL)
  {
    m_ColorNameBackground = TextDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);
}

const SimpleString* DiffDocument::evaluateLine(const DiffLine* p_pDiffLine)
{
  switch(p_pDiffLine->GetState())
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

  return &(p_pDiffLine->GetText());
}
