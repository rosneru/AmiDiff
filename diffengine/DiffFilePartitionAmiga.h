#ifndef DIFF_FILE_PARTITION_AMIGA_H
#define DIFF_FILE_PARTITION_AMIGA_H

#include "DiffFilePartition.h"
#include "SimpleString.h"

/**
 * Extends the DiffFilePartition::preprocess by Amiga file loading 
 * functionality
 *
 *
 * @author Uwe Rosner
 * @date 09/11/2018
 */
class DiffFilePartitionAmiga : public DiffFilePartition
{
public:
  bool PreProcess(const SimpleString& p_FileName);
};

#endif // DIFF_FILE_PARTITION_AMIGA_H
