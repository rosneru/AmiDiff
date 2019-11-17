#ifndef DIFF_FILE_PARTITION_LINUX_H
#define DIFF_FILE_PARTITION_LINUX_H

#include "DiffFileBase.h"

/**
 * Extends the DiffFilePartition::PreProcess with Linux file loading 
 * functionality
 *
 *
 * @author Uwe Rosner
 * @date 14/11/2018
 */
class DiffFileLinux : public DiffFileBase
{
public:
  DiffFileLinux(bool& p_bCancelRequested);
  ~DiffFileLinux();

  void Clear();

  bool PreProcess(const char* p_FileName);

  bool AddString(const char* p_String,
                 DiffLine::LineState p_LineState);
};

#endif // DIFF_FILE_PARTITION_LINUX_H
