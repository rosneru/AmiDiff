#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

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

  void AddString(const char* p_String,
                 DiffLine::LineState p_LineState);

private:
  APTR& m_pPoolHeader;
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
