#ifdef LINUX
  #include <new>
#else
  #include <new.h> // Needed for 'replacement new', see below.
#endif

#include <string.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>


#include "DiffInputFileAmiga.h"

DiffInputFileAmiga::DiffInputFileAmiga(APTR pPoolHeader,
                             bool& bCancelRequested, 
                             ProgressReporter* pProgressReporter,
                             const char* pFileName)
  : DiffFileBase(bCancelRequested),
    m_pPoolHeader(pPoolHeader)
{
  // if(m_pPoolHeader == NULL)
  // {
  //   // Won't work without memory pool
  //   m_pError = m_pErrMsgMemPool;
  //   return false;
  // }

  // if(m_pDiffLinesArray != NULL)
  // {
  //   // Already initialized
  //   return true;
  // }

  AmigaFile* m_pFile = new AmigaFile(pFileName, MODE_OLDFILE);
  
  // Initialize some variables needed for progress reporting
  int lastProgressValue = -1;
  m_NumLines = m_pFile->CountLines();

  if(m_NumLines == 0)
  {
    // Empty file
    return;
  }

  // Create an array of DiffLine-pointers to hold all needed lines
  size_t arraySize = sizeof(DiffLine*) * m_NumLines;
  m_pDiffLinesArray = (DiffLine**) AllocPooled(m_pPoolHeader, arraySize);
  if(m_pDiffLinesArray == NULL)
  {
    throw "Failed to load file.";
  }

  char* pReadLine = NULL;
  int i = 0;

  while((pReadLine = m_pFile->ReadLine()) != NULL)
  {
    char* pLine = (char*) AllocPooled(m_pPoolHeader,
                                      strlen(pReadLine) + 1);
    if(pLine == NULL)
    {
      throw "Failed to allocate memory for input lines.";
    }

    strcpy(pLine, pReadLine);

    DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                  sizeof(DiffLine));

    if(pDiffLine == NULL)
    {
      throw "Failed to allocate memory for diff lines.";
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
    if(pProgressReporter != NULL)
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
          pProgressReporter->notifyProgressChanged(lastProgressValue);
        }
      }
    }

    //
    // Handling a potential cancel request
    //
    if(m_bCancelRequested == true)
    {
      throw "User abort.";
    }
  }

  //
  // Progress reporting (final value)
  //
  if(pProgressReporter != NULL)
  {
    pProgressReporter->notifyProgressChanged(100);
  }

  delete m_pFile;
}


DiffInputFileAmiga::~DiffInputFileAmiga()
{
  // The array address is cleared but nothing else is deleted or freed
  // here, because an external memory pool is used for all heap allocs.
  // On exit or when performing another diff that memory pool is
  // deleted outside with just one call. On the Amiga this is way
  // faster than e.g. calling 5000 single delete [] in random order.

  m_pDiffLinesArray = NULL;
  m_NumLines = 0;
}


void DiffInputFileAmiga::CollectLineNumbers(size_t maxNumLines)
{
  int digits = numDigits(maxNumLines);

  for(size_t i = 0; i < m_NumLines; i++)
  {
    char* pLineNumber = (char*) AllocPooled(m_pPoolHeader, digits + 2);
    if(pLineNumber != NULL)
    {
      sprintf(pLineNumber, "%*d ", digits, (i + 1));

      DiffLine* pLine = GetLine(i);
      pLine->SetLineNum(pLineNumber);
    }
  }
}


long DiffInputFileAmiga::AddString(const char* pText,
                              DiffLine::LineState lineState,
                              const char* pFormattedLineNumber)
{
  if(m_NumLines < 1)
  {
    // Not initialized
    return -1;
  }

  if(m_pDiffLinesArray == NULL)
  {
    // Create an array of DiffLine-pointers to hold all needed lines
    size_t arraySize = sizeof(DiffLine*) * m_NumLines;
    m_pDiffLinesArray = (DiffLine**) AllocPooled(m_pPoolHeader, arraySize);
    if(m_pDiffLinesArray == NULL)
    {
      // m_pError = m_pErrMsgLowMem;
      // m_pFile.Close();
      return -1;
    }
  }

  DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                sizeof(DiffLine));

  if(pDiffLine == NULL)
  {
    // m_pError = m_pErrMsgLowMem;
    return -1;
  }

  // The next line is called 'replacement new'. It creates an object
  // of DiffLine on the known address pDiffLine and calls the
  // constructor. This has to be done here because a memory pool is
  // used and the normal operator 'new' which reserves memory
  // automatically wouldn't be appropriate.
  new (pDiffLine) DiffLine(pText, lineState, pFormattedLineNumber);

  m_pDiffLinesArray[m_NextAddedLineIdx++] = pDiffLine;

  return m_NextAddedLineIdx - 1;
}
