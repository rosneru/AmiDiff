#include "DiffDocument.h"

DiffDocument::DiffDocument(DiffFilePartition& p_DiffFilePartition)
  : m_DiffFilePartition(p_DiffFilePartition),
    m_LineId(0)
{

}

DiffDocument::~DiffDocument()
{

}

const size_t DiffDocument::NumLines() const
{
  return m_DiffFilePartition.NumLines();
}

const SimpleString* DiffDocument::GetIndexedLine(int p_LineId)
{
  m_LastScrollDirection = None;

  m_LineId = p_LineId;
  const DiffLine* pDiffLine = m_DiffFilePartition.GetDiffLine(m_LineId);

  if(pDiffLine == NULL)
  {
    m_LineColor = DiffDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);
}

DiffDocument::ColorName DiffDocument::LineColor() const
{
  return m_LineColor;
}

const SimpleString* DiffDocument::evaluateLine(const DiffLine* p_pDiffLine)
{
  switch(p_pDiffLine->State())
  {
    case DiffLine::Normal:
      m_LineColor = DiffDocument::CN_Default;
      break;

    case DiffLine::Changed:
      m_LineColor = DiffDocument::CN_Yellow;
      break;

    case DiffLine::Added:
      m_LineColor = DiffDocument::CN_Green;
      break;

    case DiffLine::Deleted:
      m_LineColor = DiffDocument::CN_Red;
      break;

    case DiffLine::Undefined:
      m_LineColor = DiffDocument::CN_Default;
      break;
  }

  return &(p_pDiffLine->Text());
}
