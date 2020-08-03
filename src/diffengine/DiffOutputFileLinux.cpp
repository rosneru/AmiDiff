
#include <string.h>


#include "DiffOutputFileLinux.h"

DiffOutputFileLinux::DiffOutputFileLinux()
{
}


DiffOutputFileLinux::~DiffOutputFileLinux()
{

}


long DiffOutputFileLinux::AddLine(const char* pText,
                                  DiffLine::LineState lineState,
                                  const char* pFormattedLineNumber)
{
  // if(m_NumLines < 1)
  // {
  //   // Not initialized
  //   return -1;
  // }

  // DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
  //                                               sizeof(DiffLine));

  // if(pDiffLine == NULL)
  // {
  //   return -1;
  // }

  // // The next line is called 'replacement new'. It creates an object
  // // of DiffLine on the known address pDiffLine and calls the
  // // constructor. This has to be done here because a memory pool is
  // // used and the normal operator 'new' which reserves memory
  // // automatically wouldn't be appropriate.
  // new (pDiffLine) DiffLine(pText, lineState, pFormattedLineNumber);

  // m_Lines.push_back(pDiffLine);

  // return m_Lines.size() - 1;
}
