#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "stdio.h"

#include "DiffLine.h"
#include "ProgressReporter.h"


/**
 * Base class for storing a file line by line with diff information.
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffFileBase
{
  // Constructor
public:
  DiffFileBase(bool& bCancelRequested);

  long NumLines() const;

  /**
   * IMPORTANT: This is and only should be used before the first
   * AddString() call when filling the difference represenation file.
   *
   * A better solution has to be found in future to avoid this
   * unnatural behavior!
   */
  void SetNumLines(long numLines);

  /**
   * Decrements the internal m_NumLines counter by 1.
   *
   * IMPORTANT: This is and only should be used from DiffEngine when
   * turning added / deleted linesinto changed lines.
   */
  void DecrementNumLines();

  DiffLine* GetLine(size_t idx) const;

  const char* GetLineText(size_t idx) const;

  const char* GetLineNum(size_t idx) const;

  DiffLine::LineState GetLineState(size_t idx) const;
  void SetLineState(size_t idx, DiffLine::LineState state);

  unsigned long GetLineToken(size_t idx) const;

  /**
   * Adds a DiffLine to file using given string and line state.
   * 
   * Returns the index where the line was inserted or -1 on error.
   */
  virtual long AddString(const char* pText, 
                         DiffLine::LineState lineState,
                         const char* pFormattedLineNumber) = 0;

  void AddBlankLine();

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* pProgressReporter);

protected:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;

  unsigned long m_NumLines;
  DiffLine** m_pDiffLinesArray;

  /**
   * The index which will be used on m_pDiffLines[] at the next
   * AddString() call.
   */
  long m_NextAddedLineIdx;

  /**
   * Returns the number of digits of a given positive number.
   */
  size_t numDigits(size_t number);

private:
  const char m_EmptyText;
};

#endif
