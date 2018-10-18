#include <clib/dos_protos.h>
#include "StopWatch.h"
#include "DosFile.h"

DosFile::DosFile()
  : MAX_LINE_LENGTH(512), // TODO A better solution needed?
    m_pLineBuf(new char[MAX_LINE_LENGTH]),
    m_pFile(NULL)
{

}

DosFile::~DosFile()
{
  if(m_pLineBuf != NULL)
  {  
    delete[] m_pLineBuf;
  }
}

bool DosFile::Open(SimpleString p_FileName, AccessMode p_AccessMode)
{
  if(m_pFile != NULL)
  {
    // There is already a file opened
    return false;
  }
  
  m_FileName = p_FileName;
  
  // Determining the corresponding accessMode for AmigaDOS
  int amigaDosAccessMode;
  switch(p_AccessMode)
  {
    case AM_NewFile:
    {
      amigaDosAccessMode = MODE_NEWFILE;
      break;
    }
    
    case AM_ReadWrite:
    {
      amigaDosAccessMode = MODE_READWRITE;
      break;
    }
    
    default:
    {
      amigaDosAccessMode = MODE_OLDFILE;
      break;
    }
  }
  
  // Opening the file
  m_pFile = ::Open(p_FileName.C_str(), amigaDosAccessMode);
  if(m_pFile == NULL)
  {
    // Opening failed
    return false;
  }
  
  return true;
}

bool DosFile::Close()
{
  ::Close(m_pFile);
  m_pFile = NULL;
  m_FileName = "";
  m_TimeStatistics = "";
}

int DosFile::CountLines()
{
  if(m_pFile == NULL)
  {
    // File not opened
    return -1;
  }
  
  int numLines = 0;
  size_t readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a
                                            // bug in AmigaOS v36/37

  // Rewind reading pointer to start of file
  Seek(m_pFile, 0, OFFSET_BEGINNING);
  
  // Initialize and start StopWatch
  StopWatch stopWatch;
  stopWatch.Start();
  
  // Reading all lines and increment counter
  while( (pBuf = FGets(m_pFile, m_pLineBuf, readBufSize)) != NULL )
  {
    numLines++;
  }
  
  // Stop StopWatch and store statistic informations
  long elapsed = static_cast<long>(stopWatch.Stop());
  m_TimeStatistics += "Counting lines of file '" + m_FileName;
  m_TimeStatistics += "': " + elapsed + " milliseconds\n";

return numLines;  
}

bool DosFile::ReadLines(LinkedList& p_List)
{
  if(m_pFile == NULL)
  {
    // File not opened
    return false;
  }
  
  size_t readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a
                                            // bug in AmigaOS v36/37

  // Rewind reading pointer to start of file
  Seek(m_pFile, 0, OFFSET_BEGINNING);
  
  // Initialize and start StopWatch
  StopWatch stopWatch;
  stopWatch.Start();
  
  // Reading all lines and increment counter
  while( (pBuf = FGets(m_pFile, m_pLineBuf, readBufSize)) != NULL )
  {
    // Read the line into a string object
    SimpleString line(m_pBuf);  // TODO optimize to save time
    SimpleString* pNewLineStr = new SimpleString(line.Trim());
    p_List->InsertTail(pNewLineStr);
  }
  
  // Stop StopWatch and store statistic informations
  long elapsed = static_cast<long>(stopWatch.Stop());
  m_TimeStatistics += "Loading lines from file '" + m_FileName;
  m_TimeStatistics += "' into a list: " + elapsed + " milliseconds\n";

  return true;
}

SimpleString DosFile::GetTimeStatistics()
{
  return m_TimeStatistics;
}