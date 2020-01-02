#include "DiffDocument.h"


DiffDocument::DiffDocument(DiffFileAmiga& leftFile,
                           const char* pLeftFileName,
                           DiffFileAmiga& rightFile,
                           const char* pRightFileName)
  : m_LeftFile(leftFile),
    m_pLeftFileName(pLeftFileName),
    m_RightFile(rightFile),
    m_pRightFileName(pRightFileName),
    m_MaxLineLength(0)
{

}

DiffDocument::~DiffDocument()
{

}

const char* DiffDocument::GetLeftFileName() const
{
  return m_pLeftFileName;
}

const char* DiffDocument::GetRightFileName() const
{
  return m_pRightFileName;
}

size_t DiffDocument::NumLines() const
{
  // Note: Right diff file should have the same number of lines
  return m_LeftFile.NumLines();
}

size_t DiffDocument::MaxLineNumChars()
{
  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  size_t maxCharsLeft = 0;
  for(size_t i = 0; i < m_LeftFile.NumLines(); i++)
  {
    if(m_LeftFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      maxCharsLeft = m_LeftFile.GetLine(i)->NumChars();
    }
  }

  size_t maxCharsRight = 0;
  for(size_t i = 0; i < m_RightFile.NumLines(); i++)
  {
    if(m_RightFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      maxCharsRight = m_RightFile.GetLine(i)->NumChars();
    }
  }

  if(maxCharsLeft > maxCharsRight)
  {
    m_MaxLineLength = maxCharsLeft;
  }
  else
  {
    m_MaxLineLength = maxCharsRight;
  }

  return m_MaxLineLength;
}


const DiffLine* DiffDocument::GetLeftLine(size_t index)
{
  m_LastScrollDirection = None;

  const DiffLine* pDiffLine = m_LeftFile.GetLine(index);

  if(pDiffLine == NULL)
  {
    m_LineColor = DiffDocument::CN_Default;
    return NULL;
  }

  return evaluateLine(pDiffLine);
}


const DiffLine* DiffDocument::GetRightLine(size_t index)
{
  m_LastScrollDirection = None;

  const DiffLine* pDiffLine = m_RightFile.GetLine(index);

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
