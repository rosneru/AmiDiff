#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include <vector>

#include "DiffInputFileAmiga.h"
#include "DiffOutputFileAmiga.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "MemoryPool.h"
#include "ProgressReporter.h"
#include "SimpleString.h"
#include "StopWatch.h"

/**
 * The document wich holds the result of the compare operation and can
 * be displayed in a DiffWindow.
 * 
 * @author Uwe Rosner
 * @date 24/09/2018
 */
class DiffDocument
{
public:
  /**
   * Creating the document immediately starts comparing the files.
   */
  DiffDocument(const char* pLeftFilePath,
               const char* pRightFilePath,
               bool& bCancelRequested,
               StopWatch& stopWatch,
               ProgressReporter& progress);

  virtual ~DiffDocument();

  const char* LeftFileName() const;
  const char* RightFileName() const;
  
  long DiffTime() const;
  size_t NumAdded() const;
  size_t NumChanged() const;
  size_t NumDeleted() const;

  size_t NumLines() const;
  size_t MaxLineLength();
  
  const DiffLine* LeftLine(size_t index) const;
  const DiffLine* RightLine(size_t index) const;

  size_t FirstDiffIndex();
  size_t LastDiffIndex();
  size_t NextDiffIndex();
  size_t PrevDiffIndex();

private:
  SimpleString m_LeftFileName;
  SimpleString m_RightFileName;
  MemoryPool m_Pool; // Ensure Pool is created before the DiffFiles.
  DiffInputFileAmiga m_LeftSrcFile;
  DiffInputFileAmiga m_RightSrcFile;
  DiffOutputFileAmiga m_LeftDiffFile;
  DiffOutputFileAmiga m_RightDiffFile;
  std::vector<size_t> m_DiffIndices;
  size_t m_DiffIndicesIdx;
  DiffEngine m_DiffEngine;
  
  long m_DiffTime;
  size_t m_NumAdded;
  size_t m_NumChanged;
  size_t m_NumDeleted;

  size_t m_MaxLineLength;
};

#endif // DIFF_DOCUMENT_H
