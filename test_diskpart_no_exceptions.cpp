#include <dos/dos.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <stdio.h>
#include <string.h>

bool testDiskPart(char const* pDir, char const* pFile)
{
  bool result = false;

  size_t bufLen = strlen(pDir);
  bufLen += strlen(pFile);
  bufLen += 2; // One for delimiter (AddPart) and one for final \0.

  STRPTR pFullPathBuf = (STRPTR) AllocVec(bufLen, MEMF_PUBLIC|MEMF_CLEAR);
  if(pFullPathBuf == NULL)
  {
    return false;
  }

  // Copy dir to destination buffer
  strcpy(pFullPathBuf, pDir);

  // Add file to destination buffer
  if(AddPart(pFullPathBuf, pFile, bufLen) == DOSTRUE)
  {
    result = true;
  }

  FreeVec(pFullPathBuf);
  return result;
}


int main(void)
{
  char const* pPath = "SYS:s";
  char const* pFile = "startup-sequence";
  if(testDiskPart(pPath, pFile) == true)
  {
    printf("DiskPart() worked.\n");
  }
  else
  {
    printf("DiskPart() failed.\n");
  }

  return RETURN_OK;
}