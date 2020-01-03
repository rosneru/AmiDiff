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

  const char* GetLeftFileName() const;
  const char* GetRightFileName() const;
  
  size_t NumLines() const;
  size_t MaxLineLength();
  
  const DiffLine* GetLeftLine(size_t index);
  const DiffLine* GetRightLine(size_t index);

private:
  DiffFileAmiga& m_LeftFile;
  const char* m_pLeftFileName;
  DiffFileAmiga& m_RightFile;
  const char* m_pRightFileName;

  char* m_FileName;
  size_t m_MaxLineLength;
};

#endif // DIFF_DOCUMENT_H
