#include <clib/dos_protos.h>
#include <libraries/dos.h>

#include "StopWatch.h"
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
  // TODO Remove it to some better place
  BPTR pFile = ::Open(p_FileName.C_str(), MODE_OLDFILE);
  if(pFile == NULL)
  {
    return false;
  }

  char pLineBuf[MAX_LINE_LENGTH];
  size_t readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a
                                            // bug in AmigaOS v36/37
  char* pBuf = NULL;

  LONG numLines = 0;
  StopWatch stopWatch;
  stopWatch.Start();

  while( (pBuf = FGets(pFile, pLineBuf, readBufSize)) != NULL )
  {
    // Read the line into a string object
    SimpleString line(pBuf);
    SimpleString* pNewLineStr = new SimpleString(line.Trim());
    m_pLines->InsertTail(pNewLineStr);

    numLines++;
  }

  // TODO store it somewhere
  LONG elapsed = stopWatch.Stop();

  ::Close(pFile);
  return true;
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