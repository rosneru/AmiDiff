#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

#include "DiffFilePartition.h"
#include "SimpleString.h"

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
  bool PreProcess(const SimpleString& p_FileName);
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
