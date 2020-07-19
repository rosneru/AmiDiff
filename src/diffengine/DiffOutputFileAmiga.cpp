#ifdef LINUX
  #include <new>
#else
  #include <new.h> // Needed for 'replacement new', see below.
#endif

#include <string.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>


#include "DiffOutputFileAmiga.h"

DiffOutputFileAmiga::DiffOutputFileAmiga(APTR& pPoolHeader,
                             bool& bCancelRequested,
                             ProgressReporter* pProgressReporter)
  : DiffFileBase(bCancelRequested),
    m_pPoolHeader(pPoolHeader)
{
}


DiffOutputFileAmiga::~DiffOutputFileAmiga()
{
  // The array address is cleared but nothing else is deleted or freed
  // here, because an external memory pool is used for all heap allocs.
  // On exit or when performing another diff that memory pool is
  // deleted outside with just one call. On the Amiga this is way
  // faster than e.g. calling 5000 single delete [] in random order.

  m_pDiffLinesArray = NULL;
  m_NumLines = 0;
}


long DiffOutputFileAmiga::AddString(const char* pText,
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
      // m_pFile->Close();
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
