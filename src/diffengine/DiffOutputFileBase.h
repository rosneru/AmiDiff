#ifndef DIFF_OUTPUT_FILE_BASE_H
#define DIFF_OUTPUT_FILE_BASE_H

#include "DiffFileBase.h"

class DiffOutputFileBase : public DiffFileBase
{
public:
  DiffOutputFileBase();
  virtual ~DiffOutputFileBase();

  /**
   * Adds a DiffLine to file using given string and line state.
   * 
   * Returns the index where the line was inserted or -1 on error.
   */
  virtual long AddLine(const char* pText, 
                       DiffLine::LineState lineState,
                       const char* pFormattedLineNumber) = 0;


  void AddBlankLine();
};

#endif
