#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include "AmigaFile.h"

AmigaFile::AmigaFile(const char* pFileName, ULONG accessMode)
  : MAX_LINE_LENGTH(1024), // TODO A better solution needed?
    m_FileDescriptor(0),
    m_FileName(pFileName),
    m_pProgressReporter(NULL)
{
  m_pLineBuf = (STRPTR) AllocVec(MAX_LINE_LENGTH, 0L);

  // Opening the file
  m_FileDescriptor = Open(pFileName, accessMode);
  if(m_FileDescriptor == 0)
  {
    throw "Failed to open file.";
  }
}


AmigaFile::~AmigaFile()
{
  if(m_pLineBuf != NULL)
  {
    FreeVec(m_pLineBuf);
    m_pLineBuf = NULL;
  }

  if(m_FileDescriptor != 0)
  {
    Close(m_FileDescriptor);
    m_FileDescriptor = 0;
  }
}


ULONG AmigaFile::CountLines()
{
  if(m_FileDescriptor == 0)
  {
    // File not opened
    return 0;
  }

  ULONG numLines = 0;
  size_t readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a
                                            // bug in AmigaOS v36/37

  // Rewind reading pointer to start of file
  Seek(m_FileDescriptor, 0, OFFSET_BEGINNING);

  // Reading all lines and increment counter
  while(FGets(m_FileDescriptor, m_pLineBuf, readBufSize) != NULL)
  {
    numLines++;
  }

  // Rewind reading pointer to start of file again
  Seek(m_FileDescriptor, 0, OFFSET_BEGINNING);

  return numLines;
}


ULONG AmigaFile::GetSize()
{
  if(m_FileDescriptor == 0)
  {
    // File not opened
    return 0;
  }

  Seek(m_FileDescriptor, 0, OFFSET_BEGINING);
  ULONG size = Seek(m_FileDescriptor, 0, OFFSET_END);
  Seek(m_FileDescriptor, 0, OFFSET_BEGINING);

  return size;
}


bool AmigaFile::ReadLines(std::vector<SimpleString*>& linesVector)
{
  // Initialize some variables needed for progress reporting
  int lastProgressValue = -1;
  int numLines = 0;
  if(m_pProgressReporter != NULL)
  {
    numLines = CountLines();
  }

  // Rewind reading pointer to start of file
  Seek(m_FileDescriptor, 0, OFFSET_BEGINNING);

  // ****
  // Initially clearing the Array
  // TODO: Implement Clear() method in Array
  // ****

  // Reading all lines and increment counter
  SimpleString line;
  int i = 0;
  while(ReadLine(line))
  {
    linesVector.push_back(new SimpleString(line));
    i++;

    //
    // Progress reporting
    //
    if(m_pProgressReporter != NULL)
    {
      // Report the 'lastProgressValue - 1' to ensure that the final
      // value of 100 (%) is sent after the last line is read.
      int newProgressValue = (i * 100 / numLines) - 1;

      if(newProgressValue > lastProgressValue)
      {
       // For performance reasons only report 5% steps
       if(newProgressValue % 5 == 0)
        {
          lastProgressValue = newProgressValue;
          m_pProgressReporter->notifyProgressChanged(lastProgressValue);
        }
      }
    }
  }

  //
  // Progress reporting (final value)
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(100);
  }

  // Rewind reading pointer to start of file
  Seek(m_FileDescriptor, 0, OFFSET_BEGINNING);

  return true;
}


bool AmigaFile::ReadLine(SimpleString& line)
{
  if(ReadLine() == NULL)
  {
    return false;
  }

  line = m_pLineBuf;
  return true;
}


char* AmigaFile::ReadLine()
{
  if(m_FileDescriptor == 0)
  {
    // File not opened
    return NULL;
  }

  ULONG readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a OS v36 failure

  if(FGets(m_FileDescriptor, m_pLineBuf, readBufSize) == NULL)
  {
    return NULL;
  }

  for(size_t i = 0; i < readBufSize; i++)
  {
    if((m_pLineBuf[i] == '\r') ||
       (m_pLineBuf[i] == '\n'))
    {
      m_pLineBuf[i] = 0;
      break;
    }
  }

  return m_pLineBuf;
}


void AmigaFile::SetProgressReporter(ProgressReporter* pProgressReporter)
{
  m_pProgressReporter = pProgressReporter;
}
