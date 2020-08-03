#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "stdio.h"
#include <vector>

#include "DiffLine.h"


/**
 * Base class for storing a file line by line with diff information.
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffFileBase
{
  friend class DiffEngine;

public:
  DiffFileBase();

  size_t NumLines() const;

  DiffLine* GetLine(size_t idx) const;

  const char* GetLineText(size_t idx) const;

  const char* GetLineNum(size_t idx) const;

  DiffLine::LineState GetLineState(size_t idx) const;
  void SetLineState(size_t idx, DiffLine::LineState state);

  unsigned long GetLineToken(size_t idx) const;

protected:
  size_t m_NumLines;
  std::vector<DiffLine*> m_Lines;


private:
  const char m_EmptyText;

  /**
   * IMPORTANT: This is only used by friend class DiffEngine. There it
   * is called before the first AddString() call to initialize the diff
   * file's m_NumLines value to the m_NumLines of according source file
   * + the initial number of differences.
   *
   * Subsequently, when converting deleted / inserted lines to changed
   * lines, the DiffEngine will probably reduce this initial m_NumLines
   * value by calling @ref decrementNumLines.
   */
  void setNumLines(size_t numLines);

  /**
   * Decrements the internal m_NumLines counter by 1.
   *
   * IMPORTANT: This is only used from the DiffEngine when changing
   * added / deleted lines into changed lines.
   */
  void decrementNumLines();
};

#endif
