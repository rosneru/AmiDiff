#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include <vector>

#include "DiffInputFileAmiga.h"
#include "DiffOutputFileAmiga.h"
#include "DiffLine.h"
#include "ProgressReporter.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  DiffDocument(const char* pLeftFilePath,
               const char* pRightFilePath,
               bool& bCancelRequested,
               ProgressReporter* pProgressReporter);

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
  const char* m_pLeftFileName;
  const char* m_pRightFileName;
  APTR m_pPoolHeader;
  DiffInputFileAmiga* m_pLeftSrcFile;
  DiffInputFileAmiga* m_pRightSrcFile;
  DiffOutputFileAmiga* m_pLeftDiffFile;
  DiffOutputFileAmiga* m_pRightDiffFile;
  std::vector<size_t> m_DiffIndices;
  
  long m_DiffTime;
  size_t m_NumAdded;
  size_t m_NumChanged;
  size_t m_NumDeleted;

  size_t m_MaxLineLength;
  size_t m_DiffIndicesIdx;
};

#endif // DIFF_DOCUMENT_H
