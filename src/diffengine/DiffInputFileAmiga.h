#ifndef DIFF_INPUT_FILE_AMIGA_H
#define DIFF_INPUT_FILE_AMIGA_H

#include <exec/types.h>

#include "AmigaFile.h"
#include "DiffFileBase.h"
#include "ProgressReporter.h"

/**
 * Extends the DiffFilePartition::PreProcess with Amiga file loading
 * functionality
 *
 *
 * @author Uwe Rosner
 * @date 18/07/2020
 */
class DiffInputFileAmiga : public DiffFileBase
{
public:
  DiffInputFileAmiga(APTR pPoolHeader, 
                     bool& bCancelRequested, 
                     ProgressReporter* pProgressReporter,
                     const char* pFileName);
  virtual ~DiffInputFileAmiga();

  void CollectLineNumbers(size_t maxNumLines);

  /**
   * Adds a DiffLine to file using given string and line state.
   * 
   * Returns the index where the line was inserted or -1 on error.
   */
  long AddString(const char* pText,
                 DiffLine::LineState lineState, 
                 const char* pFormattedLineNumber);

private:
  APTR m_pPoolHeader;
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
