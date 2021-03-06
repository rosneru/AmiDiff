#include <new>  // Needed for 'replacement new', see below.

#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
#endif

#include <string.h>


#include "AmigaFile.h"
#include "DiffInputFileAmiga.h"

DiffInputFileAmiga::DiffInputFileAmiga(APTR pPoolHeader,
                                       bool& isCancelRequested,
                                       ProgressReporter& progress,
                                       const char* pProgressDescription,
                                       const char* pFileName,
                                       bool lineNumbersEnabled)
  : DiffInputFileBase(isCancelRequested),
    m_pPoolHeader(pPoolHeader),
    m_pFileBuffer(NULL)
{
  progress.SetDescription(pProgressDescription);
  int formerProgress = 1;
  progress.SetValue(formerProgress);

  // Create the opened input file
  AmigaFile m_File(pFileName, MODE_OLDFILE);

  // Get file size and read whole file
  size_t fileBytes = m_File.ByteSize();
  m_pFileBuffer = static_cast<char*>(AllocPooled(pPoolHeader, fileBytes + 1));
  if(m_pFileBuffer == NULL)
  {
    throw "Failed to allocate memory for file buffer.";
  }

  if(m_File.ReadFile(m_pFileBuffer, fileBytes) == false)
  {
    throw "Failed to read file.";
  }

  // Create DiffLine objects for each line in file buffer
  char* pLineStart = m_pFileBuffer;
  for(size_t i = 0; i < fileBytes; i++)
  {
    if(m_pFileBuffer[i] == '\n')
    {
      // Handle a potential cancel request
      if(m_IsCancelRequested == true)
      {
        throw "User abort.";
      }

      // Finalize the current line
      m_pFileBuffer[i] = 0;

      // Create DiffLine from curren line
      DiffLine* pDiffLine = (DiffLine*) AllocPooled(m_pPoolHeader,
                                                    sizeof(DiffLine));

      if(pDiffLine == NULL)
      {
        throw "Failed to allocate memory for diff lines.";
      }

      // The next line is called 'replacement new'. It creates an object
      // of DiffLine on the known address pDiffLine and calls the
      // constructor. Here this must be used because the memory pool
      // doesn't work with normal 'new' operator.
      new (pDiffLine) DiffLine(pLineStart);

      // Append DiffLine to list
      m_Lines.push_back(pDiffLine);

      // Next line starts after current line and thin finalizing '\0'
      pLineStart = m_pFileBuffer + i + 1;

      // Progress reporting: Report the 'progressValue - 1' to ensure
      // that the final value of 100 (%) is sent after the last line is
      // read.
      int progressVal = (pLineStart - m_pFileBuffer) * 100 / fileBytes - 1;
      if(progressVal != formerProgress)
      {
        formerProgress = progressVal;

        progress.SetValue(progressVal);
      }
    }
  }

  if(lineNumbersEnabled)
  {
    collectLineNumbers(getNumLines());
  }

  progress.SetValue(100);
}


DiffInputFileAmiga::~DiffInputFileAmiga()
{

}


void DiffInputFileAmiga::collectLineNumbers(size_t maxNumLines)
{
  int digits = numDigits(maxNumLines);

  for(size_t i = 0; i < getNumLines(); i++)
  {
    char* pLineNumber = (char*) AllocPooled(m_pPoolHeader, digits + 2);
    if(pLineNumber == NULL)
    {
       throw "Failed to allocate memory for lines numbers.";
    }

    sprintf(pLineNumber, "%*d ", digits, (i + 1));

    DiffLine* pLine = (*this)[i];
    pLine->setLineNumText(pLineNumber);

    if(m_IsCancelRequested == true)
    {
      throw "User abort.";
    }
  }
}
