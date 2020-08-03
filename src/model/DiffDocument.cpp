
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include "DiffDocument.h"
#include "DiffEngine.h"

DiffDocument::DiffDocument(const char* pLeftFilePath,
                           const char* pRightFilePath,
                           bool& bCancelRequested,
                           StopWatch& stopWatch,
                           ProgressReporter& progress,
                           bool lineNumbersEnabled)
  : m_LeftSrcFile(m_Pool.Header(),
                  bCancelRequested,
                  progress,
                  "Loading left file",
                  pLeftFilePath,
                  lineNumbersEnabled),
    m_RightSrcFile(m_Pool.Header(),
                   bCancelRequested,
                   progress,
                   "Loading right file",
                   pRightFilePath,
                   lineNumbersEnabled),
    m_LeftDiffFile(m_Pool.Header()),
    m_RightDiffFile(m_Pool.Header()),
    m_DiffIndices(),
    m_DiffIndicesIdx(0),
    m_DiffEngine(m_LeftSrcFile,
                 m_RightSrcFile,
                 m_LeftDiffFile,
                 m_RightDiffFile,
                 progress,
                 "Comparing the files",
                 bCancelRequested,
                 m_DiffIndices),
    m_LineNumbersEnabled(lineNumbersEnabled),
    m_DiffTime(0),
    m_MaxLineLength(0)
{
  m_LeftFileName = pLeftFilePath;
  m_RightFileName = pRightFilePath;  
  m_DiffTime = stopWatch.Pick();
}

DiffDocument::~DiffDocument()
{

}

const char* DiffDocument::LeftFileName() const
{
  return m_LeftFileName.c_str();
}

const char* DiffDocument::RightFileName() const
{
  return m_RightFileName.c_str();
}

long DiffDocument::DiffTime() const
{
  return m_DiffTime;
}

size_t DiffDocument::NumAdded() const
{
  return m_DiffEngine.NumAdded();
}

size_t DiffDocument::NumChanged() const
{
  return m_DiffEngine.NumChanged();
}

size_t DiffDocument::NumDeleted() const
{
  return m_DiffEngine.NumDeleted();
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

size_t DiffDocument::NumDifferences() const
{
  return m_DiffEngine.NumDifferences();
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

bool DiffDocument::LineNumbersEnabled() const
{
  return m_LineNumbersEnabled;
}
