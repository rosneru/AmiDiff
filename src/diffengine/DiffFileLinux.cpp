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
    DiffLine* pItem = m_Lines[i];
    if(pItem != NULL)
    {
      if(!pItem->IsLinked() && (pItem->Text() != NULL))
      {
        delete[] pItem->Text();
        if(pItem->LineNum() != NULL)
        {
          delete[] pItem->LineNum();
        }
      }

      delete pItem;
      pItem = NULL;
    }
  }

  delete[] m_Lines;
  m_Lines = NULL;
}


bool DiffFileLinux::PreProcess(const char* pFileName)
{
  if(m_Lines != NULL)
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

  m_Lines = new DiffLine*[m_NumLines];

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
    m_Lines[i++] = pDiffLine;
  }

  inputFileStream.close();

  return NumLines() > 0;
}

void DiffFileLinux::CollectLineNumbers(size_t maxNumLines)
{
  int digits = numDigits(maxNumLines);

  for(size_t i = 0; i < m_NumLines; i++)
  {
    char* pLineNumber = new char[digits + 1];
    sprintf(pLineNumber, "%*d", digits, (i + 1));

    DiffLine* pLine = GetLine(i);
    pLine->SetLineNum(pLineNumber);
  }
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

  if(m_Lines == NULL)
  {
    m_Lines = new DiffLine*[m_NumLines];
  }

  DiffLine* pDiffLine = new DiffLine(pText,
                                     lineState,
                                     pFormattedLineNumber);

  if(pDiffLine == NULL)
  {
    return -1;
  }

  m_Lines[m_NextAddedLineIdx++] = pDiffLine;
  return m_NextAddedLineIdx - 1;
}
