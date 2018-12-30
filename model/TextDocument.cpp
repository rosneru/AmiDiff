#include "AmigaFile.h"
#include "TextDocument.h"
#include <stdio.h>

TextDocument::TextDocument()
  : m_MaxLineLength(0),
    m_LastScrollDirection(None)
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
  if(NumLines() < 1)
  {
    return;
  }

  const SimpleString* pLine = m_Lines.Pop();
  while (pLine != NULL)
  {
    delete pLine;
    pLine = m_Lines.Pop();
  }

  m_FileName = "";
}

size_t TextDocument::NumLines() const
{
  return m_Lines.Size();
}

size_t TextDocument::MaxLineLength()
{
  if(m_MaxLineLength > 0)
  {
    return m_MaxLineLength;
  }

  for(size_t i = 0; i < NumLines(); i++)
  {
    if(m_Lines[i]->Length() > m_MaxLineLength)
    {
      m_MaxLineLength = m_Lines[i]->Length();
    }
  }

  return m_MaxLineLength;
}


const SimpleString* TextDocument::GetIndexedLine(int p_LineIdx)
{
  m_LastScrollDirection = None;
  return m_Lines[p_LineIdx];
}
