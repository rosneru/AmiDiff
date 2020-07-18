#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

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
 * @date 09/11/2018
 */
class DiffFileAmiga : public DiffFileBase
{
public:
  DiffFileAmiga(APTR& pPoolHeader, 
                bool& bCancelRequested, 
                ProgressReporter* pProgressReporter);
  virtual ~DiffFileAmiga();

  void Clear();

  /**
   * Adds a DiffLine to file using given string and line state.
   * 
   * Returns the index where the line was inserted or -1 on error.
   */
  long AddString(const char* pText,
                 DiffLine::LineState lineState, 
                 const char* pFormattedLineNumber);

private:
  AmigaFile* m_pFile;
  APTR& m_pPoolHeader;

  DiffLine* createDiffLine();
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
