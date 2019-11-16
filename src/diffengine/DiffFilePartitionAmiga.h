#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

#include "DiffFilePartition.h"

/**
 * Extends the DiffFilePartition::PreProcess with Amiga file loading
 * functionality
 *
 *
 * @author Uwe Rosner
 * @date 09/11/2018
 */
class DiffFilePartitionAmiga : public DiffFilePartition
{
public:
  DiffFilePartitionAmiga(bool& p_bCancelRequested);
  ~DiffFilePartitionAmiga();

  void Clear();

  bool PreProcess(const char* pFileName);

  void AddString(const char* p_String,
                 DiffLine::LineState p_LineState);
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
