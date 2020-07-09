#include <string.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <libraries/asl.h>
#include "CmdFileRequester.h"


CmdFileRequester::CmdFileRequester(Array<WindowBase*>& windowArray,
                                   const char* pTitle)
  : CommandBase(windowArray),
    m_pTitle(pTitle)
{

}

CmdFileRequester::~CmdFileRequester()
{

}

void CmdFileRequester::Execute(struct Window* pActiveWindow)
{
  // Disable the "About..." item (this command) in all menus
  DisableInAllWindowMenus();

  // Set the busy pointer for all windows
  enableBusyPointerForAllWindows();

  // Display the about dialog
  selectFile(pActiveWindow);

  // Set the normal pointer for all windows
  disableBusyPointerForAllWindows();

  // Enable the "About..." item (this command) in all menus
  EnableInAllWindowMenus();
}


void CmdFileRequester::SetPreselectPathOnly(bool yes)
{
  m_bPreselectPathOnly = yes;
}


void CmdFileRequester::SetInitialFilePath(const char* pInitialPath)
{
  m_InitialFilePath = pInitialPath;
}


const SimpleString& CmdFileRequester::SelectedFile() const
{
  return m_SelectedFilePath;
}


void CmdFileRequester::selectFile(struct Window* pActiveWindow)
{
  if(m_pTitle == NULL)
  {
    return;
  }

  m_SelectedFilePath = "";

  SimpleString initialFilePart = "";
  SimpleString initialPathPart = "";
  if(m_InitialFilePath.Length() > 0)
  {
    // Extract path and file name from initial full file name path
    const char* pFullPath = m_InitialFilePath.C_str();
    const char* pPathPart = PathPart(pFullPath);
    const char* pFilePart = FilePart(pFullPath);

    size_t pathLen = pPathPart - pFullPath;
    if(pathLen > 0)
    {
      initialPathPart = m_InitialFilePath.SubStr(0, pathLen);
    }

    initialFilePart = pFilePart;
  }

  if(m_bPreselectPathOnly)
  {
    // Only take the path-part for pre-selection, not the file name
    initialFilePart = "";
  }

  // Allocate data structure for the ASL requester
  struct FileRequester* pFileRequest = (struct FileRequester*)
    AllocAslRequestTags(ASL_FileRequest,
                        ASL_Hail, (ULONG) m_pTitle,
                        ASL_Dir, (ULONG) initialPathPart.C_str(),
                        ASL_File, (ULONG) initialFilePart.C_str(),
                        TAG_DONE);

  if(pFileRequest == NULL)
  {
    // Data struct allocation failed
    return;
  }

  // Open the file requester and wait until the user selected a file
  if(AslRequestTags(pFileRequest,
                    ASLFR_Window, (ULONG)pActiveWindow,
                    ASLFR_RejectIcons, TRUE,
                    TAG_DONE) == FALSE)
  {
    // Requester opening failed
    FreeAslRequest(pFileRequest);
    return;
  }

  // Copying selected path name into a big enough buffer
  size_t bufLen = strlen(pFileRequest->fr_Drawer);
  bufLen += strlen(pFileRequest->fr_File);
  bufLen += 2; // One for delimiter (AddPart) and one for final \0.
  STRPTR pFullPathBuf = (STRPTR) AllocVec(bufLen, MEMF_PUBLIC);
  if(pFullPathBuf == NULL)
  {
    FreeAslRequest(pFileRequest);
    return;
  }
  
  strcpy(pFullPathBuf, pFileRequest->fr_Drawer);

  // Calling a dos.library function to combine path and file name
  if(AddPart(pFullPathBuf, pFileRequest->fr_File, bufLen) == FALSE)
  {
    FreeVec(pFullPathBuf);
    FreeAslRequest(pFileRequest);
    return;
  }

  m_SelectedFilePath = pFullPathBuf;

  FreeVec(pFullPathBuf);
  FreeAslRequest(pFileRequest);
}
