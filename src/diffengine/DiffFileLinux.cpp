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

  for(size_t i = 0; i < m_NumLines; i++)
  {
    DiffLine* pItem = m_pDiffLinesArray[i];
    if(pItem != NULL)
    {
      if(!pItem->IsLinked() && (pItem->Text() != NULL))
      {
        delete[] pItem->Text();
        delete[] pItem->LineNum();
      }

      delete pItem;
      pItem = NULL;
    }
  }

  delete[] m_pDiffLinesArray;
  m_pDiffLinesArray = NULL;
}


bool DiffFileLinux::PreProcess(const char* pFileName, 
                               bool bCollectLineNumbers)
{
  if(m_pDiffLinesArray != NULL)
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

  m_pDiffLinesArray = new DiffLine*[m_NumLines];

  try
  {
    inputFileStream.open(pFileName);
  }
  catch (std::exception& e)
  {
    // Can't open file
    return false;
  }

  size_t digits = numDigits(m_NumLines);

  std::string line;
  int i = 0;
  while(getline(inputFileStream, line))
  {
    std::size_t size = line.length();
    char* pLine = new char[size + 1];
    strcpy(pLine, line.c_str());

    char* pLineNumber = NULL;
    if(bCollectLineNumbers == true)
    {
      pLineNumber = new char[digits + 1];
      sprintf(pLineNumber, "%*d", digits, (i + 1));
    }

    DiffLine* pDiffLine = new DiffLine(pLine, pLineNumber);
    if(pDiffLine == NULL)
    {
      break;
    }

    // Append DiffLine to list
    m_pDiffLinesArray[i++] = pDiffLine;
  }

  inputFileStream.close();

  return NumLines() > 0;
}

long DiffFileLinux::AddString(const char* pText,
                              DiffLine::LineState lineState,
                              const char* pFormattedLineNumber)
{
  if(m_NumLines < 1)
  {
    // Not initialized
    return -1;
  }

  if(m_pDiffLinesArray == NULL)
  {
    m_pDiffLinesArray = new DiffLine*[m_NumLines];
  }

  DiffLine* pDiffLine = new DiffLine(pText,
                                     lineState,
                                     pFormattedLineNumber);

  if(pDiffLine == NULL)
  {
    return -1;
  }

  m_pDiffLinesArray[m_NextAddedLineIdx++] = pDiffLine;
  return m_NextAddedLineIdx - 1;
}
