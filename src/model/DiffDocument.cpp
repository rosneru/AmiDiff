#include "DiffDocument.h"


DiffDocument::DiffDocument(DiffFileAmiga& p_DiffFile)
  : m_DiffFile(p_DiffFile),
    m_LineId(0)
{

}

DiffDocument::~DiffDocument()
{

}

size_t DiffDocument::NumLines() const
{
  return m_DiffFile.NumLines();
}

size_t DiffDocument::MaxLineLength()
{
  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  for(size_t i = 0; i < NumLines(); i++)
  {
    if(m_DiffFile.GetLine(i)->Length() > m_MaxLineLength)
    {
      m_MaxLineLength = m_DiffFile.GetLine(i)->Length();
    }
  }

  return m_MaxLineLength;
}


const DiffLine* DiffDocument::GetIndexedLine(int p_LineId)
{
  m_LastScrollDirection = None;

  m_LineId = p_LineId;
  const DiffLine* pDiffLine = m_DiffFile.GetLine(m_LineId);

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

const DiffLine* DiffDocument::evaluateLine(const DiffLine* pDiffLine)
{
  switch(pDiffLine->State())
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

  return pDiffLine;
}
