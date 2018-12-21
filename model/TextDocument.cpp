#include "AmigaFile.h"
#include "TextDocument.h"
#include <stdio.h>

TextDocument::TextDocument()
  : m_LastScrollDirection(None)
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
  m_LastScrollDirection = PreviousLine;
  return static_cast<SimpleString*>(m_Lines.GetPrev());
}

const SimpleString* TextDocument::GetNextLine()
{
  m_LastScrollDirection = NextLine;
  return static_cast<SimpleString*>(m_Lines.GetNext());
}

const SimpleString* TextDocument::GetIndexedLine(int p_LineIdx)
{
  m_LastScrollDirection = None;
  return static_cast<SimpleString*>(m_Lines.GetIndexed(p_LineIdx));
}
