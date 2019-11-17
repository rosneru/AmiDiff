#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <string.h>

#include "DiffFileLinux.h"

DiffFileLinux::DiffFileLinux(
  bool& p_bCancelRequested)
  : DiffFileBase(p_bCancelRequested)
{
}

DiffFileLinux::~DiffFileLinux()
{
  Clear();
}

void DiffFileLinux::Clear()
{
  if(m_DiffLinesArray.Size() == 0)
  {
    return;
  }

  DiffLine* pItem;
  while((pItem = m_DiffLinesArray.Pop()) != NULL)
  {
    if(!pItem->TextIsLinked() && (pItem->Text() != NULL))
    {
      delete[] pItem->Text();
    }

    delete pItem;

    if(m_DiffLinesArray.Size() == 0)
    {
      break;
    }
  }
}


bool DiffFileLinux::PreProcess(const char* pFileName)
{
  std::ifstream inputFileStream;
  try
  {
    inputFileStream.open(pFileName);
  }
  catch (std::exception& e)
  {
    return false;
  }

  std::string line;
  while(getline(inputFileStream, line))
  {
    std::size_t size = line.length();
    char* pLine = new char[size + 1];
    strcpy(pLine, line.c_str());

    DiffLine* pDiffLine = new DiffLine(pLine);
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

bool DiffFileLinux::AddString(const char* p_String,
                                       DiffLine::LineState p_LineState)
{
  DiffLine* pDiffLine = new DiffLine(p_String, p_LineState);
  if(pDiffLine == NULL)
  {
    return false;
  }

  m_DiffLinesArray.Push(pDiffLine);
}
