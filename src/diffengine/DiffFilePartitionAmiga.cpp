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
  int lastProgressValue = 0;
  int numLines = file.CountLines();
  if(numLines < 1)
  {
    return true;
  }


  SimpleString line;
  int i = 0;
  while(file.ReadLine(line))
  {
	  AddString(line);
	  i++;

    if(m_pProgressReporter != NULL)
    {
      int newProgressValue = i / numLines;
      if(newProgressValue > lastProgressValue)
      {
        lastProgressValue = newProgressValue;

        // Report the lastProgressValue-1 to ensure that the final
        // progress value of 100 (%) is sent after the last line is
        // read.
        m_pProgressReporter->notifyProgressChanged(lastProgressValue - 1);
      }
    }

  }

  if(m_pProgressReporter != NULL)
  {
    // Now reporting the final progress value
    m_pProgressReporter->notifyProgressChanged(100);
  }


  file.Close();

  return true;
}
