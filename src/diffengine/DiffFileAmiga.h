#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

#include <exec/types.h>

#include "AmigaFile.h"
#include "DiffFileBase.h"

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
  DiffFileAmiga(APTR& pPoolHeader, bool& bCancelRequested);
  ~DiffFileAmiga();

  void Clear();

  bool PreProcess(const char* pFileName, 
                  bool bCollectLineNumbers = false);

  /**
   * Adds a DiffLine to file using given string and line state.
   * 
   * Returns the index where the line was inserted or -1 on error.
   */
  long AddString(const char* pText,
                 DiffLine::LineState lineState, 
                 const char* pFormattedLineNumber);

  const char* Error();

private:
  AmigaFile m_File;
  APTR& m_pPoolHeader;
  const char* m_pErrMsgLowMem;
  const char* m_pErrMsgMemPool;
  const char* m_pErrMsgUnknown;
  const char* m_pError;

  DiffLine* createDiffLine();
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
