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
  std::vector<DiffLine*> m_Lines;

  size_t numDigits(size_t number);


private:
  const char m_EmptyText;
};

#endif
