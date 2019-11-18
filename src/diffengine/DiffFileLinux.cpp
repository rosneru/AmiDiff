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
  if(m_NumLines == 0)
  {
    return;
  }

  for(int i = 0; i < m_NumLines; i++)
  {
    DiffLine* pItem = m_pDiffLines[i];
    if(pItem != NULL)
    {
      if(!pItem->TextIsLinked() && (pItem->Text() != NULL))
      {
        delete[] pItem->Text();
      }

      delete pItem;
      pItem = NULL;
    }
  }

  delete[] m_pDiffLines;
  m_pDiffLines = NULL;
}


bool DiffFileLinux::PreProcess(const char* pFileName)
{
  if(m_pDiffLines != NULL)
  {
    // ALready initialized
    return true;
  }

  std::ifstream inputFileStream;
  try
  {
    inputFileStream.open(pFileName);
  }
  catch (std::exception& e)
  {
    // Can't open file
    return false;
  }

  m_NumLines = 0;
  std::string s;
  while(getline(inputFileStream, s))
  {
    m_NumLines++;
  }
  inputFileStream.close();

  if(m_NumLines == 0)
  {
    // Empty file
    return false;
  }

  m_pDiffLines = new DiffLine*[m_NumLines];

  try
  {
    inputFileStream.open(pFileName);
  }
  catch (std::exception& e)
  {
    // Can't open file
    return false;
  }

  std::string line;
  int i = 0;
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
    m_pDiffLines[i++] = pDiffLine;
  }

  inputFileStream.close();

  return NumLines() > 0;
}

bool DiffFileLinux::AddString(const char* p_String,
                              DiffLine::LineState p_LineState)
{
  if(m_NumLines < 1)
  {
    // Not initialized
    return false;
  }

  if(m_pDiffLines == NULL)
  {
    m_pDiffLines = new DiffLine*[m_NumLines];
  }

  DiffLine* pDiffLine = new DiffLine(p_String, p_LineState);
  if(pDiffLine == NULL)
  {
    return false;
  }

  m_pDiffLines[m_NextAddedLineIdx++] = pDiffLine;
}
