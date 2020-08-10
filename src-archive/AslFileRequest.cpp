#include <string.h>

#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <libraries/asl.h>
#include <libraries/dos.h>

#include "AslFileRequest.h"

AslFileRequest::AslFileRequest(struct Window*& pIntuiWindow)
  : m_pIntuiWindow(pIntuiWindow)
{

}

AslFileRequest::~AslFileRequest()
{

}

std::string AslFileRequest::SelectFile(const std::string& title,
  const std::string& initialFileFullPath,
  bool bPreselectPathOnly)
{
  std::string selectedFileFullPath = "";

  std::string initialFilePart = "";
  std::string initialPathPart = "";
  if(initialFileFullPath.length() > 0)
  {
    // Extract path and file name from initial full file name path
    const char* pFullPath = initialFileFullPath.c_str();
    const char* pPathPart = PathPart(pFullPath);
    const char* pFilePart = FilePart(pFullPath);

    size_t pathLen = pPathPart - pFullPath;
    if(pathLen > 0)
    {
      initialPathPart = initialFileFullPath.substr(0, pathLen);
    }

    initialFilePart = pFilePart;
  }

  if(bPreselectPathOnly)
  {
    // Only take the path-part for pre-selection, not the file name
    initialFilePart = "";
  }

  // Allocate data structure for the ASL requester
  struct FileRequester* pFileRequest = (struct FileRequester*)
    AllocAslRequestTags(ASL_FileRequest,
                        ASL_Hail, (ULONG) title.c_str(),
                        ASL_Dir, (ULONG) initialPathPart.c_str(),
                        ASL_File, (ULONG) initialFilePart.c_str(),
                        TAG_DONE);

  if(pFileRequest == NULL)
  {
    // Data struct allocation failed
    return selectedFileFullPath;
  }

  // Open the file requester and wait until the user selected a file
  if(AslRequestTags(pFileRequest,
                    ASLFR_Window, (ULONG)m_pIntuiWindow,
                    ASLFR_RejectIcons, TRUE,
                    TAG_DONE) == FALSE)
  {
    // Requester opening failed
    FreeAslRequest(pFileRequest);
    return selectedFileFullPath;
  }

  // Copying selected path name into a big enough buffer
  // TODO Find something better than use a fixed buffer size. Is there
  //      e.g. a system max path length defined somewhere?
  int bufLen = 2048;
  STRPTR pFullPathBuf = (STRPTR) AllocVec(bufLen, 0L);
  if(pFullPathBuf != NULL)
  {
    strcpy(pFullPathBuf, pFileRequest->rf_Dir);

    // Calling a dos.library function to combine path and file name
    if(AddPart(pFullPathBuf, pFileRequest->fr_File, bufLen))
    {
      selectedFileFullPath = pFullPathBuf;
    }

    FreeVec(pFullPathBuf);
  }

  FreeAslRequest(pFileRequest);

  return selectedFileFullPath;
}
