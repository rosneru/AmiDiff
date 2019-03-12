#ifndef DIFF_FILE_PARTITION_LINUX_H
#define DIFF_FILE_PARTITION_LINUX_H

#include "DiffFilePartition.h"
#include "SimpleString.h"

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
  bool PreProcess(const SimpleString& p_FileName);
};

#endif // DIFF_FILE_PARTITION_LINUX_H
