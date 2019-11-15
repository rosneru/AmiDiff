#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "DiffFilePartitionLinux.h"

DiffFilePartitionLinux::DiffFilePartitionLinux(
  bool& p_bCancelRequested)
  : DiffFilePartition(p_bCancelRequested)
{
}


bool DiffFilePartitionLinux::PreProcess(const SimpleString& p_FileName)
{
  ifstream inputFileStream;
  try
  {
    inputFileStream.open(p_FileName.C_str());
  }
  catch (exception& e)
  {
    return false;
  }

  std::string line;
  while(getline(inputFileStream, line))
  {
    DiffLine* pDiffLine = new DiffLine(line.c_str());
    if(pDiffLine == NULL)
    {
      break;
    }

    // Append DiffLine to list
    m_DiffLinesArray.Push(pDiffLine);

  }

  inputFileStream.close();

  return NumLines() > 0;
}
