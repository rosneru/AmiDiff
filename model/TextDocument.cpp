#include "DosFile.h"
#include "TextDocument.h"


TextDocument::TextDocument()
  : m_pLines(new LinkedList())
{
  SimpleString* pInitial = new SimpleString();
  m_pLines->InsertHead(pInitial);
  m_pCurrentLine = static_cast<SimpleString*>(GetCurrentLine());
}

TextDocument::~TextDocument()
{
  Clear();
  delete m_pLines;
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
  if(p_FileName.Length)
  {
    return false;
  }

  // Open file
  DosFile dosFile;
  if(dosFile.Open(p_FileName.C_str(), DosFile::AM_OldFile) == false)
  {
    return false;
  }
  
  // Read line by line into list
  bool bSuccess = dosFile.ReadLines(*m_pLines);
  if(bSuccess == true)
  {
    GetFirstLine();
  }

  m_TimeStatistics += dosFile.GetTimeStatistics();
  return bSuccess;
}

void TextDocument::Clear()
{
  while (GetFirstLine())
  {
    delete m_pCurrentLine;
    m_pLines->RemoveItem();
  }
}

SimpleString* TextDocument::GetFirstLine()
{
  m_pCurrentLine = static_cast<SimpleString*>(m_pLines->GetFirst());
  return m_pCurrentLine;
}

SimpleString* TextDocument::GetCurrentLine()
{
  m_pCurrentLine = static_cast<SimpleString*>(m_pLines->GetSelected());
  return m_pCurrentLine;
}

SimpleString* TextDocument::GetPreviousLine()
{
  m_pCurrentLine = static_cast<SimpleString*>(m_pLines->GetPrev());
  return m_pCurrentLine;
}

SimpleString* TextDocument::GetNextLine()
{
  m_pCurrentLine = static_cast<SimpleString*>(m_pLines->GetNext());
  return m_pCurrentLine;
}

SimpleString* TextDocument::GetIndexedLine(int p_lineIdx)
{
  m_pCurrentLine = static_cast<SimpleString*>(m_pLines->GetIndexed(p_lineIdx));
  return m_pCurrentLine;
}

size_t TextDocument::NumLines()
{
  return m_pLines->Size();
}
