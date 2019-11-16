#include <clib/dos_protos.h>
#include <clib/exec_protos.h>

#include "AmigaFile.h"
#include "DiffFileAmiga.h"

DiffFileAmiga::DiffFileAmiga(APTR& pPoolHeader, 
                             bool& p_bCancelRequested)
  : DiffFileBase(p_bCancelRequested),
    m_pPoolHeader(pPoolHeader)
{
}

DiffFileAmiga::~DiffFileAmiga()
{
  Clear();
}

void DiffFileAmiga::Clear()
{
  // Nothing is deleted or freed here, because an external memory pool 
  // is used for all allocations. On exit or when performing another 
  // diff this memory pool is deleted outside with just one call.
  // On the Amiga this is way faster than e.g. calling 5000 single 
  // delete [] in random order.
}

bool DiffFileAmiga::PreProcess(const char* pFileName)
{
  if(m_pPoolHeader == NULL)
  {
    // Won't work without memory pool
    return false;
  }

  AmigaFile file;
  if(!file.Open(pFileName, AmigaFile::AM_OldFile))
  {
  	return false;
  }

  // Initialize some variables needed for progress reporting
  int lastProgressValue = -1;
  int numLines = 0;
  if(m_pProgressReporter != NULL)
  {
    numLines = file.CountLines();
  }

  char* pReadLine = NULL;
  int i = 0;
  while((pReadLine = file.ReadLine()) != NULL)
  {
    char* pLine = (char*) AllocPooled(m_pPoolHeader, 
                                      strlen(pReadLine) + 1);

    strcpy(pLine, pReadLine);

    DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader, 
                                                  sizeof(DiffLine));

    // Because of using a memory pool the constructor has to be called manually
    new (pDiffLine) DiffLine(pLine);

    if(pDiffLine == NULL)
    {
      break;
    }

    // Append DiffLine to list
    m_DiffLinesArray.Push(pDiffLine);

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
      m_bCancelRequested = false;
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


  file.Close();

  return true;
}


void DiffFileAmiga::AddString(const char* p_String,
                              DiffLine::LineState p_LineState)
{
  DiffLine* pDiffLine = new DiffLine(p_String, p_LineState);
  if(pDiffLine == NULL)
  {
    return;
  }

  m_DiffLinesArray.Push(pDiffLine);
}
