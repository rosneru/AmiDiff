#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include "AmigaFile.h"

AmigaFile::AmigaFile()
  : MAX_LINE_LENGTH(512), // TODO A better solution needed?
    m_pFile(NULL),
    m_pProgressReporter(NULL)
{
  m_pLineBuf = (STRPTR) AllocVec(MAX_LINE_LENGTH, 0L);
}

AmigaFile::~AmigaFile()
{
  if(m_pLineBuf != NULL)
  {
    FreeVec(m_pLineBuf);
  }
}

bool AmigaFile::Open(const SimpleString& p_FileName,
  AccessMode p_AccessMode)
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

void AmigaFile::Close()
{
  ::Close(m_pFile);
  m_pFile = NULL;
  m_FileName = "";
}

int AmigaFile::CountLines()
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

  // Reading all lines and increment counter
  while(FGets(m_pFile, m_pLineBuf, readBufSize) != NULL)
  {
    numLines++;
  }

  // Rewind reading pointer to start of file again
  Seek(m_pFile, 0, OFFSET_BEGINNING);

  return numLines;
}

ULONG AmigaFile::GetSize()
{
  if(m_pFile == NULL)
  {
    // File not opened
    return 0;
  }

  Seek(m_pFile, 0, OFFSET_BEGINING);
  ULONG size = Seek(m_pFile, 0, OFFSET_END);
  Seek(m_pFile, 0, OFFSET_BEGINING);

  return size;
}

bool AmigaFile::ReadLines(Array<SimpleString*>& p_Array)
{
  if(m_pFile == NULL)
  {
    // File not opened
    return false;
  }

  // Initialize some variables needed for progress reporting
  int lastProgressValue = -1;
  int numLines = 0;
  if(m_pProgressReporter != NULL)
  {
    numLines = CountLines();
  }

  // Rewind reading pointer to start of file
  Seek(m_pFile, 0, OFFSET_BEGINNING);

  // ****
  // Initially clearing the Array
  // TODO: Implement Clear() method in Array
  // ****

  // Reading all lines and increment counter
  SimpleString line;
  int i = 0;
  while(ReadLine(line))
  {
    p_Array.Push(new SimpleString(line));
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
  Seek(m_pFile, 0, OFFSET_BEGINNING);

  return true;
}

bool AmigaFile::ReadLine(SimpleString& p_Line)
{
  if(m_pFile == NULL)
  {
    // File not opened
    return false;
  }

  ULONG readBufSize = MAX_LINE_LENGTH - 1; // -1 => Workaround for a OS v36 failure

  if(FGets(m_pFile, m_pLineBuf, readBufSize) == NULL)
  {
    return false;
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

  p_Line = m_pLineBuf;
  return true;
}

void AmigaFile::SetProgressReporter(ProgressReporter* p_pProgressReporter)
{
  m_pProgressReporter = p_pProgressReporter;
}
