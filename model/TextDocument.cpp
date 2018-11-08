#include "DosFile.h"
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
  DosFile dosFile;
  if(dosFile.Open(p_FileName.C_str(), DosFile::AM_OldFile) == false)
  {
    return false;
  }

  // Read line by line into list
  bool bSuccess = dosFile.ReadLines(m_Lines);
  if(bSuccess == true)
  {
    m_FileName = p_FileName;
  }

  m_TimeStatistics += dosFile.GetTimeStatistics();
  dosFile.Close();
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


const SimpleString* TextDocument::GetFirstLine() const
{
  return static_cast<SimpleString*>(m_Lines.GetFirst());
}

const SimpleString* TextDocument::GetCurrentLine() const
{
  return static_cast<SimpleString*>(m_Lines.GetSelected());
}

const SimpleString* TextDocument::GetPreviousLine() const
{
  return static_cast<SimpleString*>(m_Lines.GetPrev());
}

const SimpleString* TextDocument::GetNextLine() const
{
  return static_cast<SimpleString*>(m_Lines.GetNext());
}

const SimpleString* TextDocument::GetIndexedLine(int p_LineIdx) const
{
  return static_cast<SimpleString*>(m_Lines.GetIndexed(p_LineIdx));
}
