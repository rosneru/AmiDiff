#include "DosFile.h"
#include "TextDocument.h"


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
bool TextDocument::Load(SimpleString p_FileName)
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
  SimpleString* pLine = GetFirstLine();
  while (pLine != NULL)
  {
    delete pLine;
    m_Lines.RemoveItem();
    pLine = GetFirstLine();
  }

  m_FileName = "";
  m_TimeStatistics = "";
}

const size_t TextDocument::NumLines()
{
  return m_Lines.Size();
}


SimpleString* TextDocument::GetFirstLine()
{
  return static_cast<SimpleString*>(m_Lines.GetFirst());
}

SimpleString* TextDocument::GetCurrentLine()
{
  return static_cast<SimpleString*>(m_Lines.GetSelected());
}

SimpleString* TextDocument::GetPreviousLine()
{
  return static_cast<SimpleString*>(m_Lines.GetPrev());
}

SimpleString* TextDocument::GetNextLine()
{
  return static_cast<SimpleString*>(m_Lines.GetNext());
}

SimpleString* TextDocument::GetIndexedLine(int p_LineIdx)
{
  return static_cast<SimpleString*>(m_Lines.GetIndexed(p_LineIdx));
}
