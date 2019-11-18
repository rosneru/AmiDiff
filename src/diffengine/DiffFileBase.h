#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "DiffLine.h"
#include "ProgressReporter.h"


/**
 * Class for storing a file line by line with diff information.
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffFileBase
{
  // Constructor
public:
  DiffFileBase(bool& p_bCancelRequested);

  long NumLines() const;

  /**
   * IMPORTANT: This is and only has to be used before the first
   * AddString() call when filling the difference represenation file.
   *
   * A better solution has to be found in future to avoid this
   * unnatural behavior!
   */
  void SetNumLines(long numLines);

  void NumChanges(int& p_Added, int& p_Changed, int& p_Deleted) const;

  DiffLine* GetLine(size_t p_Index) const;
  const char* GetLineText(size_t p_Index) const;
  unsigned long GetLineToken(size_t p_Index) const;
  DiffLine::LineState GetLineState(size_t p_Index) const;
  void SetLineState(size_t p_Index, DiffLine::LineState state);

  virtual bool AddString(const char*, DiffLine::LineState) = 0;
  void AddBlankLine();

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

protected:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;

  unsigned long m_NumLines;
  DiffLine** m_pDiffLines;

  /**
   * The index which will be used on m_pDiffLines[] at the next
   * AddString() call.
   */
  long m_NextAddedLineIdx;
};

#endif
