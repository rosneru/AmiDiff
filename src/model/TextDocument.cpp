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
bool TextDocument::Load(const char* pFileName)
{
  if(pFileName == NULL)
  {
    return false;
  }

  return false;
  // TODO Re-implement with char* instead SimpleString
  
  // Clear();

  // // Open file
  // AmigaFile file;
  // if(file.Open(pFileName, AmigaFile::AM_OldFile) == false)
  // {
  //   return false;
  // }

  // // Read line by line into list
  // bool bSuccess = file.ReadLines(m_Lines);
  // if(bSuccess == true)
  // {
  //   m_FileName = pFileName;
  // }

  // file.Close();
  // return bSuccess;

}

void TextDocument::Clear()
{
  if(m_Lines.Size() == 0)
  {
    return;
  }

  const SimpleString* pLine;
  while((pLine = m_Lines.Pop()) != NULL)
  {
    delete pLine;

    if(m_Lines.Size() == 0)
    {
      break;
    }
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


const char* TextDocument::GetIndexedLine(int idx)
{
  m_LastScrollDirection = None;
  return m_Lines[idx];
}
