#include "AmigaFile.h"
#include "TextDocument.h"
#include <stdio.h>

TextDocument::TextDocument()
{
}

TextDocument::~TextDocument()
{
  Clear();
}

/**
 * @brief Loading a file to internal list
 *
 *
 * @param pFileName
 * Name of text file to load
 *
 */
bool TextDocument::Load(const SimpleString& p_FileName)
{
  if(p_FileName.Length() == 0)
  {
    return false;
  }

  Clear();

  // Open file
  AmigaFile file;
  if(file.Open(p_FileName.C_str(), AmigaFile::AM_OldFile) == false)
  {
    return false;
  }

  // Read line by line into list
  bool bSuccess = file.ReadLines(m_Lines);
  if(bSuccess == true)
  {
    m_FileName = p_FileName;
  }

  m_TimeStatistics += file.GetTimeStatistics();
  file.Close();
  return bSuccess;
}

void TextDocument::Clear()
{
  const SimpleString* pLine = GetFirstLine();
  while (pLine != NULL)
  {
    delete pLine;
    m_Lines.RemoveItem();
    pLine = GetFirstLine();
  }

  m_FileName = "";
  m_TimeStatistics = "";
}

const size_t TextDocument::NumLines() const
{
  return m_Lines.Size();
}


const SimpleString* TextDocument::GetFirstLine()
{
  return static_cast<SimpleString*>(m_Lines.GetFirst());
}

const SimpleString* TextDocument::GetCurrentLine()
{
  return static_cast<SimpleString*>(m_Lines.GetSelected());
}

const SimpleString* TextDocument::GetPreviousLine()
{
  return static_cast<SimpleString*>(m_Lines.GetPrev());
}

const SimpleString* TextDocument::GetNextLine()
{
  return static_cast<SimpleString*>(m_Lines.GetNext());
}

const SimpleString* TextDocument::GetIndexedLine(int p_LineIdx)
{
  return static_cast<SimpleString*>(m_Lines.GetIndexed(p_LineIdx));
}
