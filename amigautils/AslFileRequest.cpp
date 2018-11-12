#include <string.h>

#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <libraries/asl.h>
#include <libraries/dos.h>

#include "AslFileRequest.h"

AslFileRequest::AslFileRequest(struct Window* p_pIntuiWindow)
  : m_pIntuiWindow(p_pIntuiWindow)
{

}

AslFileRequest::~AslFileRequest()
{

}

SimpleString AslFileRequest::SelectFile(const SimpleString& p_Title, 
  const SimpleString& p_InitialFileFullPath)
{
  SimpleString selectedFileFullPath = "";
  
  SimpleString initialFilePart = "";
  SimpleString initialPathPart = "";
  if(p_InitialFileFullPath.Length() > 0)
  {
    initialPathPart = PathPart(p_InitialFileFullPath.C_str());
    initialFilePart = FilePart(p_InitialFileFullPath.C_str());
  }

  // Allocate data structure for the ASL requester
  struct FileRequester* pFileRequest = (struct FileRequester*)
    AllocAslRequestTags(
      ASL_FileRequest,
      ASL_Hail, (ULONG) p_Title.C_str(),
      ASL_Dir, (ULONG) initialPathPart.C_str(), 
      ASL_File, (ULONG) initialFilePart.C_str(), 
      TAG_DONE);

  if(pFileRequest == NULL)
  {
    // Data struct allocation failed
    return selectedFileFullPath;
  }

  // Open the file requester and wait until the user selected a file
  if(AslRequestTags(pFileRequest, ASLFR_Window, (ULONG)m_pIntuiWindow,
    TAG_DONE) == FALSE)
  {
    // Requester opening failed
    FreeAslRequest(pFileRequest);
    return selectedFileFullPath;
  }

  // Copying selected path name into a big enough buffer
  // TODO Find something better than use a fixed buffer size. Is there
  //      e.g. a system max path length defined somewhere?
  char fullPathBuf[512];
  strcpy(fullPathBuf, pFileRequest->rf_Dir);

  // Calling a dos.library function to combine path and file name
  if(AddPart(fullPathBuf, pFileRequest->fr_File, 512))
  {
   selectedFileFullPath = fullPathBuf;
  }

  FreeAslRequest(pFileRequest);

  return selectedFileFullPath;
}
