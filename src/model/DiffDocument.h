#ifndef DIFF_DOCUMENT_H
#define DIFF_DOCUMENT_H

#include "DiffFileAmiga.h"
#include "DiffLine.h"
#include "SimpleString.h"

class DiffDocument
{
public:
  /**
   * Defines the text color names used to set foreground and
   * background pens for text rendering
   */
  enum ColorName
  {
    CN_Default,
    CN_Red,
    CN_Green,
    CN_Yellow
  };

  /**
   * Used to describe if the last line obtaining method was
   * getNextLine(), getPreviousLine() ore none of both.
   */
  enum LastScrollDirection
  {
    None,
    NextLine,
    PreviousLine,
  };

  DiffDocument(DiffFileAmiga& leftFile,
               const char* pLeftFileName,
               DiffFileAmiga& rightFile,
               const char* pRightFileName);

  virtual ~DiffDocument();

  const char* GetLeftFileName() const;
  const char* GetRightFileName() const;
  
  size_t NumLines() const;
  size_t MaxLineNumChars();
  
  const DiffLine* GetLeftLine(size_t index);
  const DiffLine* GetRightLine(size_t index);

  ColorName LineColor() const;

private:
  DiffFileAmiga& m_LeftFile;
  const char* m_pLeftFileName;
  DiffFileAmiga& m_RightFile;
  const char* m_pRightFileName;

  ColorName m_LineColor;
  const DiffLine* evaluateLine(const DiffLine* pDiffLine);

  char* m_FileName;
  size_t m_MaxLineLength;

  /**
   * Stores if the last scroll line obtaining method was getNextLine(),
   * getPreviousLine() ore none of both.
   */
  LastScrollDirection m_LastScrollDirection;
};

#endif // DIFF_DOCUMENT_H
