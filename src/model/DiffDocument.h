#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffOutputFileAmiga.h"
#include "DiffLine.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  DiffDocument(DiffOutputFileAmiga& leftFile,
               const char* pLeftFileName,
               DiffOutputFileAmiga& rightFile,
               const char* pRightFileName);

  virtual ~DiffDocument();

  const char* LeftFileName() const;
  const char* RightFileName() const;
  
  size_t NumLines() const;
  size_t MaxLineLength();
  
  const DiffLine* LeftLine(size_t index) const;
  const DiffLine* RightLine(size_t index) const;

private:
  DiffOutputFileAmiga& m_LeftFile;
  const char* m_pLeftFileName;
  DiffOutputFileAmiga& m_RightFile;
  const char* m_pRightFileName;

  char* m_FileName;
  size_t m_MaxLineLength;
};

#endif // DIFF_DOCUMENT_H
