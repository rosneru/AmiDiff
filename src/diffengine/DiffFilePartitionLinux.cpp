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
    SimpleString simpleString(line.c_str());
    AddString(simpleString);
  }

  inputFileStream.close();

  return NumLines() > 0;
}
