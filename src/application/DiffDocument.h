#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include <list>
#include <string>

#include "DiffInputFileAmiga.h"
#include "DiffOutputFileAmiga.h"
#include "DiffEngine.h"
#include "DiffLine.h"
#include "MemoryPool.h"
#include "ProgressReporter.h"
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
               bool& isCancelRequested,
               StopWatch& stopWatch,
               ProgressReporter& progress,
               bool lineNumbersEnabled);

  virtual ~DiffDocument();

  const char* LeftFileName() const;
  const char* RightFileName() const;
  
  long DiffTime() const;
  size_t NumAdded() const;
  size_t NumChanged() const;
  size_t NumDeleted() const;

  size_t NumLines() const;

  size_t MaxLineLength() const;

  const DiffLine* LeftLine(size_t index) const;
  const DiffLine* RightLine(size_t index) const;

  const DiffOutputFileBase& LeftDiffFile() const;
  const DiffOutputFileBase& RightDiffFile() const;

  size_t getNumDifferences() const;

  size_t NextDiffIndex();
  size_t PrevDiffIndex();

  bool LineNumbersEnabled() const;

private:
  std::string m_LeftFileName;
  std::string m_RightFileName;
  MemoryPool m_Pool; // Ensure Pool is created before the DiffFiles.
  DiffInputFileAmiga m_LeftSrcFile;
  DiffInputFileAmiga m_RightSrcFile;
  DiffOutputFileAmiga m_LeftDiffFile;
  DiffOutputFileAmiga m_RightDiffFile;

  std::list<size_t> m_DiffIndices;

  DiffEngine m_DiffEngine;

  std::list<size_t>::iterator m_DiffIndicesIterator;

  bool m_LineNumbersEnabled;
  long m_DiffTime;
  size_t m_MaxLineLength;
};

#endif
