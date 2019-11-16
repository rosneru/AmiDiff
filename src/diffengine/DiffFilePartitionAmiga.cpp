#include <clib/dos_protos.h>

#include "AmigaFile.h"
#include "DiffFilePartitionAmiga.h"

DiffFilePartitionAmiga::DiffFilePartitionAmiga(
  bool& p_bCancelRequested)
  : DiffFilePartition(p_bCancelRequested)
{
}

DiffFilePartitionAmiga::~DiffFilePartitionAmiga()
{
  Clear();
}

void DiffFilePartitionAmiga::Clear()
{
  if(m_DiffLinesArray.Size() == 0)
  {
    return;
  }

  DiffLine* pItem;
  while((pItem = m_DiffLinesArray.Pop()) != NULL)
  {
    if(!pItem->TextIsLinked() && (pItem->Text() != NULL))
    {
      delete[] pItem->Text();
    }

    delete pItem;

    if(m_DiffLinesArray.Size() == 0)
    {
      break;
    }
  }
}

bool DiffFilePartitionAmiga::PreProcess(const char* pFileName)
{
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
    char* pLine = new char[strlen(pReadLine) + 1];
    strcpy(pLine, pReadLine);

    DiffLine* pDiffLine = new DiffLine(pLine);
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


void DiffFilePartitionAmiga::AddString(const char* p_String,
                                       DiffLine::LineState p_LineState)
{
  DiffLine* pDiffLine = new DiffLine(p_String, p_LineState);
  if(pDiffLine == NULL)
  {
    return;
  }

  m_DiffLinesArray.Push(pDiffLine);
}
