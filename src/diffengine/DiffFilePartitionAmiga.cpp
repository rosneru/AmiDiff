#include <proto/dos.h>

#include "AmigaFile.h"
#include "DiffFilePartitionAmiga.h"

bool DiffFilePartitionAmiga::PreProcess(const SimpleString& p_FileName)
{
  AmigaFile file;
  if(!file.Open(p_FileName, AmigaFile::AM_OldFile))
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

  SimpleString line;
  int i = 0;
  while(file.ReadLine(line))
  {
	  AddString(line);
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

    // TODO remove after implementation of ProgressWindow
    Delay(1);

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
