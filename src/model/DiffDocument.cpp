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

const char* DiffDocument::LeftFileName() const
{
  return m_pLeftFileName;
}

const char* DiffDocument::RightFileName() const
{
  return m_pRightFileName;
}

size_t DiffDocument::NumLines() const
{
  // Note: Right diff file should have the same number of lines
  return m_LeftFile.NumLines();
}

size_t DiffDocument::MaxLineLength()
{
  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  for(size_t i = 0; i < m_LeftFile.NumLines(); i++)
  {
    if(m_LeftFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_LeftFile.GetLine(i)->NumChars();
    }
  }

  for(size_t i = 0; i < m_RightFile.NumLines(); i++)
  {
    if(m_RightFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_RightFile.GetLine(i)->NumChars();
    }
  }

  return m_MaxLineLength;
}


const DiffLine* DiffDocument::LeftLine(size_t index) const
{
  return m_LeftFile.GetLine(index);
}


const DiffLine* DiffDocument::RightLine(size_t index) const
{
  return m_RightFile.GetLine(index);
}
