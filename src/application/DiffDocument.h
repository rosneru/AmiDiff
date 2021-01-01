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

  const char* getLeftFileName() const;
  const char* getRightFileName() const;
  
  long getElapsedDiffTime() const;

  size_t getNumDifferences() const;
  size_t NumAdded() const;
  size_t NumChanged() const;
  size_t NumDeleted() const;

  size_t getNumLines() const;

  size_t getMaxLineLength() const;

  const DiffOutputFileBase& getLeftDiffFile() const;
  const DiffOutputFileBase& getRightDiffFile() const;

  size_t getNextDiffLineId();
  size_t getPrevDiffLineId();

  bool areLineNumbersEnabled() const;

private:
  std::string m_LeftFileName;
  std::string m_RightFileName;
  MemoryPool m_Pool; // Ensure the pool is created before the DiffFiles
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
