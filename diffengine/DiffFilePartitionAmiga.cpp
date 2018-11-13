#include "AmigaFile.h"
#include "DiffFilePartitionAmiga.h"

bool DiffFilePartitionAmiga::PreProcess(const SimpleString& p_FileName)
{
  AmigaFile file;
  if(!file.Open(p_FileName, AmigaFile::AM_OldFile))
  {
  	return false;
  }

  SimpleString line;
  while(file.ReadLine(line))
  {
	  AddString(new SimpleString(line));
  }

  file.Close();

  return true;
}
