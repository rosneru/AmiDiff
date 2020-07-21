
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include "DiffDocument.h"
#include "DiffEngine.h"
#include "StopWatch.h"

DiffDocument::DiffDocument(const char* pLeftFilePath,
                           const char* pRightFilePath,
                           bool& bCancelRequested,
                           ProgressReporter* pProgressReporter)
  : m_pPoolHeader(NULL),
    m_pLeftSrcFile(NULL),
    m_pRightSrcFile(NULL),
    m_pLeftDiffFile(NULL),
    m_pRightDiffFile(NULL),
    m_DiffTime(0),
    m_NumAdded(0),
    m_NumChanged(0),
    m_NumDeleted(0),
    m_MaxLineLength(0),
    m_DiffIndices(),
    m_DiffIndicesIdx(0)
{
  // Create the memory pool
  m_pPoolHeader = CreatePool(MEMF_ANY|MEMF_CLEAR, 50000, 25000);
  if(m_pPoolHeader == NULL)
  {
    throw "Failed to create the memory pool.";
  }

  StopWatch m_StopWatch;

  // Load and analyze the left file
  m_pLeftSrcFile = new DiffInputFileAmiga(m_pPoolHeader,
                                          bCancelRequested,
                                          pProgressReporter,
                                          pLeftFilePath);

  // Load and analyze the right file
  m_pRightSrcFile = new DiffInputFileAmiga(m_pPoolHeader,
                                            bCancelRequested,
                                            pProgressReporter,
                                            pRightFilePath);

  m_pLeftDiffFile = new DiffOutputFileAmiga(m_pPoolHeader,
                                            bCancelRequested,
                                            pProgressReporter);

  m_pRightDiffFile = new DiffOutputFileAmiga(m_pPoolHeader,
                                              bCancelRequested,
                                              pProgressReporter);

  DiffEngine diffEngine(m_pLeftSrcFile,
                        m_pRightSrcFile,
                        m_pLeftDiffFile,
                        m_pRightDiffFile,
                        bCancelRequested,
                        m_DiffIndices);

  // Compare the files
  bool diffOk = diffEngine.Diff();
  if(!diffOk)
  {
    throw "Error while performing the diff.";
  }

  m_DiffTime = m_StopWatch.Pick();
  m_NumAdded = diffEngine.NumAdded();
  m_NumChanged = diffEngine.NumChanged();
  m_NumDeleted = diffEngine.NumDeleted();
}

DiffDocument::~DiffDocument()
{
  if(m_pRightSrcFile != NULL)
  {
    delete m_pRightSrcFile;
    m_pRightSrcFile = NULL;
  }
  
  if(m_pLeftSrcFile != NULL)
  {
    delete m_pLeftSrcFile;
    m_pLeftSrcFile = NULL;
  }

  if(m_pRightDiffFile != NULL)
  {
    delete m_pRightDiffFile;
    m_pRightDiffFile = NULL;
  }

  if(m_pLeftDiffFile != NULL)
  {
    delete m_pLeftDiffFile;
    m_pLeftDiffFile = NULL;
  }

  if(m_pPoolHeader != NULL)
  {
    DeletePool(m_pPoolHeader);
    m_pPoolHeader = NULL;
  }
}

const char* DiffDocument::LeftFileName() const
{
  return m_pLeftFileName;
}

const char* DiffDocument::RightFileName() const
{
  return m_pRightFileName;
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
  if(m_pLeftDiffFile == NULL)
  {
    return 0;
  }

  // Note: Right diff file should have the same number of lines
  return m_pLeftDiffFile->NumLines();
}

size_t DiffDocument::MaxLineLength()
{
  if((m_pLeftDiffFile == NULL) || (m_pRightDiffFile == NULL))
  {
    return 0;
  }

  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  for(size_t i = 0; i < m_pLeftDiffFile->NumLines(); i++)
  {
    if(m_pLeftDiffFile->GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_pLeftDiffFile->GetLine(i)->NumChars();
    }
  }

  for(size_t i = 0; i < m_pRightDiffFile->NumLines(); i++)
  {
    if(m_pRightDiffFile->GetLine(i)->NumChars() > m_MaxLineLength)
    {
      m_MaxLineLength = m_pRightDiffFile->GetLine(i)->NumChars();
    }
  }

  return m_MaxLineLength;
}


const DiffLine* DiffDocument::LeftLine(size_t index) const
{
  if(m_pLeftDiffFile == NULL)
  {
    return NULL;
  }

  return m_pLeftDiffFile->GetLine(index);
}


const DiffLine* DiffDocument::RightLine(size_t index) const
{
  if(m_pRightDiffFile == NULL)
  {
    return NULL;
  }

  return m_pRightDiffFile->GetLine(index);
}

size_t DiffDocument::FirstDiffIndex() 
{
  if(m_DiffIndices.size() == 0)
  {
    return 0;
  }

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
    return m_DiffIndicesIdx;
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
    return m_DiffIndicesIdx;
  }

  m_DiffIndicesIdx--;
  return m_DiffIndices[m_DiffIndicesIdx];
}
