#ifdef __clang__
  #include <clib/exec_protos.h>
#else
  #include <proto/exec.h>
#endif

#include <exec/memory.h>

#include "DiffDocument.h"
#include "DiffEngine.h"

DiffDocument::DiffDocument(const char* pLeftFilePath,
                           const char* pRightFilePath,
                           bool& isCancelRequested,
                           StopWatch& stopWatch,
                           ProgressReporter& progress,
                           bool lineNumbersEnabled)
  : m_LeftSrcFile(m_Pool.Header(),
                  isCancelRequested,
                  progress,
                  "Loading left file",
                  pLeftFilePath,
                  lineNumbersEnabled),
    m_RightSrcFile(m_Pool.Header(),
                   isCancelRequested,
                   progress,
                   "Loading right file",
                   pRightFilePath,
                   lineNumbersEnabled),
    m_LeftDiffFile(m_LeftSrcFile, m_Pool.Header()),
    m_RightDiffFile(m_RightSrcFile, m_Pool.Header()),
    m_DiffEngine(m_LeftSrcFile,
                 m_RightSrcFile,
                 m_LeftDiffFile,
                 m_RightDiffFile,
                 progress,
                 "Comparing the files",
                 isCancelRequested,
                 m_DiffIndices),
    m_DiffIndicesIterator(m_DiffIndices.end()),
    m_LineNumbersEnabled(lineNumbersEnabled),
    m_DiffTime(0),
    m_MaxLineLength(0)
{
  m_LeftFileName = pLeftFilePath;
  m_RightFileName = pRightFilePath;  
  m_DiffTime = stopWatch.Pick();

  // Determine the maximum number of chars which either can be found is
  // in the left or in the right file

  for(size_t i = 0; i < m_LeftDiffFile.getNumLines(); i++)
  {
    if(m_LeftDiffFile[i]->getNumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_LeftDiffFile[i]->getNumChars();
    }
  }

  for(size_t i = 0; i < m_RightDiffFile.getNumLines(); i++)
  {
    if(m_RightDiffFile[i]->getNumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_RightDiffFile[i]->getNumChars();
    }
  }
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
  return m_DiffEngine.getNumAdded();
}

size_t DiffDocument::NumChanged() const
{
  return m_DiffEngine.getNumChanged();
}

size_t DiffDocument::NumDeleted() const
{
  return m_DiffEngine.getNumDeleted();
}


size_t DiffDocument::NumLines() const
{

  // Note: Right diff file should have the same number of lines
  return m_LeftDiffFile.getNumLines();
}

size_t DiffDocument::MaxLineLength() const
{
  return m_MaxLineLength;
}


const DiffLine* DiffDocument::LeftLine(size_t index) const
{
  return m_LeftDiffFile[index];
}


const DiffLine* DiffDocument::RightLine(size_t index) const
{
  return m_RightDiffFile[index];
}


const DiffOutputFileBase& DiffDocument::LeftDiffFile() const
{
  return m_LeftDiffFile;
}


const DiffOutputFileBase& DiffDocument::RightDiffFile() const
{
    return m_RightDiffFile;
}


size_t DiffDocument::getNumDifferences() const
{
  return m_DiffEngine.getNumDifferences();
}

size_t DiffDocument::NextDiffIndex() 
{
  if(m_DiffIndicesIterator == m_DiffIndices.end())
  {
    // Iterator points to the end. This only is true directly after
    // construction of DiffDocument. Set it to the first item.
    m_DiffIndicesIterator = m_DiffIndices.begin();
  }
  else
  {
    m_DiffIndicesIterator++;

    if(m_DiffIndicesIterator == m_DiffIndices.end())
    {
      // Avoid overflow: back to last valid item
      m_DiffIndicesIterator--;
    }
  }

  return (*m_DiffIndicesIterator);
}

size_t DiffDocument::PrevDiffIndex() 
{
  if(m_DiffIndicesIterator == m_DiffIndices.end())
  {
    // Iterator points to the end. This only is true directly after
    // construction of DiffDocument. Set it to the first item.
    m_DiffIndicesIterator = m_DiffIndices.begin();
  }
  else if(m_DiffIndicesIterator != m_DiffIndices.begin())
  {
    // Only if not already the first item
    m_DiffIndicesIterator--;
  }

  return (*m_DiffIndicesIterator);
}

bool DiffDocument::LineNumbersEnabled() const
{
  return m_LineNumbersEnabled;
}
