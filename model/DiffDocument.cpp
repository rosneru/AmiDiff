#include "StopWatch.h"
#include "DosFile.h"
#include "DiffDocument.h"

DiffDocument::DiffDocument()
  : MAX_LINE_LENGTH(512),
    m_pLines(new LinkedList())
{

}

DiffDocument::~DiffDocument()
{
  Clear();
  delete m_pLines;
}

bool DiffDocument::ReadFile(SimpleString p_FileName)
{
  if(NumLines() > 0)
  {
    Clear();
  }

  // Open file and read line by line into window
  DosFile dosFile;
  if(dosFile.Open(p_FileName.C_str(), DosFile::AM_OldFile) == false)
  {
    return false;
  }
  
  m_TimeStatistics += dosFile.GetTimeStatistics();
  return dosFile.ReadLines(*m_pLines);
}

void DiffDocument::Clear()
{
  SimpleString* pCurrentLine = NULL;
  while ((pCurrentLine = GetFirstLine()))
  {
    delete pCurrentLine;
    m_pLines->RemoveItem();
  }
}

size_t DiffDocument::NumLines()
{
  return m_pLines->Size();
}

SimpleString* DiffDocument::GetFirstLine()
{
  return static_cast<SimpleString*>(m_pLines->GetFirst());
}

SimpleString* DiffDocument::GetCurrentLine()
{
  return static_cast<SimpleString*>(m_pLines->GetSelected());
}

SimpleString* DiffDocument::GetPreviousLine()
{
  return static_cast<SimpleString*>(m_pLines->GetPrev());
}

SimpleString* DiffDocument::GetNextLine()
{
  return static_cast<SimpleString*>(m_pLines->GetNext());
}

SimpleString* DiffDocument::GetIndexedLine(int p_lineIdx)
{
  return static_cast<SimpleString*>(m_pLines->GetIndexed(p_lineIdx));
}
