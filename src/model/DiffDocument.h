#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffFileAmiga.h"
#include "DiffLine.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  DiffDocument(DiffFileAmiga& leftFile,
               const char* pLeftFileName,
               DiffFileAmiga& rightFile,
               const char* pRightFileName);

  virtual ~DiffDocument();

  const char* LeftFileName() const;
  const char* RightFileName() const;
  
  size_t NumLines() const;
  size_t MaxLineLength();
  
  const DiffLine* LeftLine(size_t index) const;
  const DiffLine* RightLine(size_t index) const;

private:
  DiffFileAmiga& m_LeftFile;
  const char* m_pLeftFileName;
  DiffFileAmiga& m_RightFile;
  const char* m_pRightFileName;

  char* m_FileName;
  size_t m_MaxLineLength;
};

#endif // DIFF_DOCUMENT_H
