#ifndef DIFF_FILE_PARTITION_LINUX_H
#define DIFF_FILE_PARTITION_LINUX_H

#include "DiffFilePartition.h"

/**
 * Extends the DiffFilePartition::PreProcess with Linux file loading 
 * functionality
 *
 *
 * @author Uwe Rosner
 * @date 14/11/2018
 */
class DiffFilePartitionLinux : public DiffFilePartition
{
public:
  DiffFilePartitionLinux(bool& p_bCancelRequested);
  ~DiffFilePartitionLinux();

  bool PreProcess(const char* p_FileName);

  void AddString(const char* p_String,
                 DiffLine::LineState p_LineState);
};

#endif // DIFF_FILE_PARTITION_LINUX_H
