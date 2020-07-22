
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include "DiffDocument.h"
#include "DiffEngine.h"

DiffDocument::DiffDocument(const char* pLeftFilePath,
                           const char* pRightFilePath,
                           bool& bCancelRequested,
                           StopWatch& stopWatch,
                           ProgressReporter* pProgressReporter)
  : m_LeftFileName(pLeftFilePath),
    m_RightFileName(pRightFilePath),
    m_LeftSrcFile(m_Pool.Header(),
                  bCancelRequested,
                  pProgressReporter,
                  pLeftFilePath),
    m_RightSrcFile(m_Pool.Header(),
                   bCancelRequested,
                   pProgressReporter,
                   pRightFilePath),
    m_LeftDiffFile(m_Pool.Header(), bCancelRequested, pProgressReporter),
    m_RightDiffFile(m_Pool.Header(), bCancelRequested, pProgressReporter),
    m_DiffIndices(),
    m_DiffIndicesIdx(0),
    m_DiffEngine(m_LeftSrcFile,
                 m_RightSrcFile,
                 m_LeftDiffFile,
                 m_RightDiffFile,
                 bCancelRequested,
                 m_DiffIndices),
    m_DiffTime(0),
    m_NumAdded(0),
    m_NumChanged(0),
    m_NumDeleted(0),
    m_MaxLineLength(0)
{
  m_DiffTime = stopWatch.Pick();
  m_NumAdded = m_DiffEngine.NumAdded();
  m_NumChanged = m_DiffEngine.NumChanged();
  m_NumDeleted = m_DiffEngine.NumDeleted();
}

DiffDocument::~DiffDocument()
{

}

const char* DiffDocument::LeftFileName() const
{
  return m_LeftFileName.C_str();
}

const char* DiffDocument::RightFileName() const
{
  return m_RightFileName.C_str();
}

long DiffDocument::DiffTime() const
{
  return m_DiffTime;
}

size_t DiffDocument::NumAdded() const
{
  return m_NumAdded;
}

size_t DiffDocument::NumChanged() const
{
  return m_NumChanged;
}

size_t DiffDocument::NumDeleted() const
{
  return m_NumDeleted;
}


size_t DiffDocument::NumLines() const
{

  // Note: Right diff file should have the same number of lines
  return m_LeftDiffFile.NumLines();
}

size_t DiffDocument::MaxLineLength()
{
  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  for(size_t i = 0; i < m_LeftDiffFile.NumLines(); i++)
  {
    if(m_LeftDiffFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_LeftDiffFile.GetLine(i)->NumChars();
    }
  }

  for(size_t i = 0; i < m_RightDiffFile.NumLines(); i++)
  {
    if(m_RightDiffFile.GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_RightDiffFile.GetLine(i)->NumChars();
    }
  }

  return m_MaxLineLength;
}


const DiffLine* DiffDocument::LeftLine(size_t index) const
{
  return m_LeftDiffFile.GetLine(index);
}


const DiffLine* DiffDocument::RightLine(size_t index) const
{
  return m_RightDiffFile.GetLine(index);
}

size_t DiffDocument::FirstDiffIndex() 
{

  m_DiffIndicesIdx = m_DiffIndices.front();
  return m_DiffIndices[m_DiffIndicesIdx];
}

size_t DiffDocument::LastDiffIndex() 
{
  if(m_DiffIndices.size() == 0)
  {
    return 0;
  }

  m_DiffIndicesIdx = m_DiffIndices.back();
  return m_DiffIndices[m_DiffIndicesIdx];
}

size_t DiffDocument::NextDiffIndex() 
{
  if(m_DiffIndices.size() == 0)
  {
    return 0;
  }

  if(m_DiffIndicesIdx > (m_DiffIndices.size() - 1))
  {
    return m_DiffIndices.back();
  }

  m_DiffIndicesIdx++;
  return m_DiffIndices[m_DiffIndicesIdx];
}

size_t DiffDocument::PrevDiffIndex() 
{
  if(m_DiffIndices.size() == 0)
  {
    return 0;
  }

  if(m_DiffIndicesIdx < 1)
  {
    return m_DiffIndices.front();
  }

  m_DiffIndicesIdx--;
  return m_DiffIndices[m_DiffIndicesIdx];
}
