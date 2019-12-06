#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include <new.h>

#include "DiffFileAmiga.h"

DiffFileAmiga::DiffFileAmiga(APTR& pPoolHeader,
                             bool& p_bCancelRequested)
  : DiffFileBase(p_bCancelRequested),
    m_pPoolHeader(pPoolHeader),
    m_pErrMsgLowMem("Not enough memory."),
    m_pErrMsgMemPool("Memory pool not initialized."),
    m_pErrMsgUnknown("Unknown error in class DiffFileAmiga."),
    m_pError(m_pErrMsgUnknown)
{
}


DiffFileAmiga::~DiffFileAmiga()
{
  Clear();
}


void DiffFileAmiga::Clear()
{
  // The array address is cleared but nothing else is deleted or freed
  // here, because an external memory pool is used for all heap allocs.
  // On exit or when performing another diff that memory pool is
  // deleted outside with just one call. On the Amiga this is way
  // faster than e.g. calling 5000 single delete [] in random order.

  m_pDiffLinesArray = NULL;
}

bool DiffFileAmiga::PreProcess(const char* pFileName)
{
  if(m_pPoolHeader == NULL)
  {
    // Won't work without memory pool
    m_pError = m_pErrMsgMemPool;
    return false;
  }

  if(m_pDiffLinesArray != NULL)
  {
    // Already initialized
    return true;
  }


  if(!m_File.Open(pFileName, AmigaFile::AM_OldFile))
  {
    m_pError = m_File.Error();
  	return false;
  }

  // Initialize some variables needed for progress reporting
  int lastProgressValue = -1;
  m_NumLines = 0;
  if(m_pProgressReporter != NULL)
  {
    m_NumLines = m_File.CountLines();
  }

  if(m_NumLines == 0)
  {
    // Empty file
    return false;
  }

  // Create an array of DiffLine-pointers to hold all needed lines
  size_t arraySize = sizeof(DiffLine*) * m_NumLines;
  m_pDiffLinesArray = (DiffLine**) AllocPooled(m_pPoolHeader, arraySize);
  if(m_pDiffLinesArray == NULL)
  {
    m_pError = m_pErrMsgLowMem;
    m_File.Close();
    return false;
  }

  char* pReadLine = NULL;
  int i = 0;
  while((pReadLine = m_File.ReadLine()) != NULL)
  {
    char* pLine = (char*) AllocPooled(m_pPoolHeader,
                                      strlen(pReadLine) + 1);
    if(pLine == NULL)
    {
      m_pError = m_pErrMsgLowMem;
      m_File.Close();
      return false;
    }

    strcpy(pLine, pReadLine);

    DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                  sizeof(DiffLine));

    if(pDiffLine == NULL)
    {
      m_pError = m_pErrMsgLowMem;
      m_File.Close();
      return false;
    }

    // The next line is called 'replacement new'. It creates an object
    // of DiffLine on the known address pDiffLine and calls the
    // constructor. This has to be done here because a memory pool is
    // used and the normal operator 'new' which reserves memory
    // automatically wouldn't be appropriate.
    new (pDiffLine) DiffLine(pLine);

    // Append DiffLine to list
    m_pDiffLinesArray[i++] = pDiffLine;

    //
    // Progress reporting
    //
    if(m_pProgressReporter != NULL)
    {
      // Report the 'lastProgressValue - 1' to ensure that the final
      // value of 100 (%) is sent after the last line is read.
      int newProgressValue = (i * 100 / m_NumLines) - 1;

      if(newProgressValue > lastProgressValue)
      {
        // For performance reasons report only 3% steps
        if(newProgressValue % 3 == 0)
        {
          lastProgressValue = newProgressValue;
          m_pProgressReporter->notifyProgressChanged(lastProgressValue);
        }
      }
    }

    //
    // Handling a potential cancel request
    //
    if(m_bCancelRequested == true)
    {
      m_File.Close();
      return false;
    }
  }

  //
  // Progress reporting (final value)
  //
  if(m_pProgressReporter != NULL)
  {
    m_pProgressReporter->notifyProgressChanged(100);
  }

  m_File.Close();
  return true;
}


bool DiffFileAmiga::AddString(const char* p_String,
                              DiffLine::LineState p_LineState)
{
  if(m_NumLines < 1)
  {
    // Not initialized
    return false;
  }

  if(m_pDiffLinesArray == NULL)
  {
    // Create an array of DiffLine-pointers to hold all needed lines
    size_t arraySize = sizeof(DiffLine*) * m_NumLines;
    m_pDiffLinesArray = (DiffLine**) AllocPooled(m_pPoolHeader, arraySize);
    if(m_pDiffLinesArray == NULL)
    {
      m_pError = m_pErrMsgLowMem;
      m_File.Close();
      return false;
    }
  }

  DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                sizeof(DiffLine));

  if(pDiffLine == NULL)
  {
    m_pError = m_pErrMsgLowMem;
    return false;
  }

  // The next line is called 'replacement new'. It creates an object
  // of DiffLine on the known address pDiffLine and calls the
  // constructor. This has to be done here because a memory pool is
  // used and the normal operator 'new' which reserves memory
  // automatically wouldn't be appropriate.
  new (pDiffLine) DiffLine(p_String, p_LineState);

  m_pDiffLinesArray[m_NextAddedLineIdx++] = pDiffLine;

  return true;
}

const char* DiffFileAmiga::Error()
{
  return m_pError;
}
