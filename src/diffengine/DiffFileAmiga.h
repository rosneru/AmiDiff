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
  DiffFileAmiga(APTR& pPoolHeader, bool& p_bCancelRequested);
  ~DiffFileAmiga();

  void Clear();

  bool PreProcess(const char* pFileName);

  bool AddString(const char* p_String,
                 DiffLine::LineState p_LineState);

  const char* Error();

private:
  AmigaFile m_File;
  APTR& m_pPoolHeader;
  const char* m_pErrMsgLowMem;
  const char* m_pErrMsgMemPool;
  const char* m_pErrMsgUnknown;
  char* m_pError;

  DiffLine* createDiffLine();
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
