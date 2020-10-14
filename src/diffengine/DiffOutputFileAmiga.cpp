#ifdef LINUX
  #include <new>
#else
  #include <new.h> // Needed for 'replacement new', see below.
#endif

#include <string.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>


#include "DiffOutputFileAmiga.h"

DiffOutputFileAmiga::DiffOutputFileAmiga(const DiffInputFileBase& diffInputFile, 
                                         APTR pPoolHeader)
  : DiffOutputFileBase(diffInputFile),
    m_pPoolHeader(pPoolHeader)
{
}


DiffOutputFileAmiga::~DiffOutputFileAmiga()
{

}


long DiffOutputFileAmiga::AddLine(const char* pText,
                                  DiffLine::LineState lineState,
                                  const char* pFormattedLineNumber)
{
  DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                sizeof(DiffLine));

  if(pDiffLine == NULL)
  {
   throw "Not enough memory to create DiffLine.";
  }

  // The next line is called 'replacement new'. It creates an object
  // of DiffLine on the known address pDiffLine and calls the
  // constructor. This has to be done here because a memory pool is
  // used and the normal operator 'new' which reserves memory
  // automatically wouldn't be appropriate.
  new (pDiffLine) DiffLine(pText, lineState, pFormattedLineNumber);

  m_Lines.push_back(pDiffLine);

  return m_Lines.size() - 1;
}
