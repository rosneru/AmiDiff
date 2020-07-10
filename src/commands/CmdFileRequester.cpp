#include <string.h>
#include <clib/asl_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
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
  m_InitialFileFullPath = pInitialPath;
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

  SimpleString initialFile = "";
  SimpleString initialPath = "";
  if(m_InitialFileFullPath.Length() > 0)
  {
    // Extract path and file name from initial full file name path
    const char* pFullPath = m_InitialFileFullPath.C_str();
    const char* pPathPart = PathPart(pFullPath);
    const char* pFilePart = FilePart(pFullPath);

    size_t pathLen = pPathPart - pFullPath;
    if(pathLen > 0)
    {
      initialPath = m_InitialFileFullPath.SubStr(0, pathLen);
    }

    initialFile = pFilePart;
  }

  if(m_bPreselectPathOnly)
  {
    // Only take the path-part for pre-selection, not the file name
    initialFile = "";
  }

  struct Hook aslHook;
  aslHook.h_Entry = reinterpret_cast<ULONG (*)()>(HookEntry);
  aslHook.h_SubEntry = reinterpret_cast<ULONG (*)()>(intuiHook);


  // Allocate data structure for the ASL requester
  struct FileRequester* pFileRequest = (struct FileRequester*)
    AllocAslRequestTags(ASL_FileRequest,
                        ASLFR_TitleText, (ULONG) m_pTitle,
                        ASLFR_InitialDrawer, (ULONG) initialPath.C_str(),
                        ASLFR_InitialFile, (ULONG) initialFile.C_str(),
                        ASLFR_Window, (ULONG) pActiveWindow,
                        ASLFR_RejectIcons, TRUE,
                        ASLFR_IntuiMsgFunc, &aslHook,
                        TAG_DONE);

  if(pFileRequest == NULL)
  {
    // Data struct allocation failed
    return;
  }

  // Open the file requester and wait until the user selected a file
  if(AslRequestTags(pFileRequest, TAG_DONE) == FALSE)
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
  if(AddPart(pFullPathBuf, pFileRequest->fr_File, bufLen) == TRUE)
  {
    // Path and file name are combined; this is the user selected path
    m_SelectedFilePath = pFullPathBuf;
  }

  FreeVec(pFullPathBuf);
  FreeAslRequest(pFileRequest);
}


void CmdFileRequester::intuiHook(struct Hook* pHook,
                                 struct FileRequester* pFileRequester,
                                 struct IntuiMessage* pMsg)
{
  switch (pMsg->Class)
  {
    // One of the windows has been resized
    // case IDCMP_NEWSIZE:
    //   for(size_t i = 0; i < m_Windows.Size(); i++)
    //   {
    //     if(m_Windows[i]->IntuiWindow() == pMsg->IDCMPWindow)
    //     {
    //       // Re-paint the resized window
    //       m_Windows[i]->Resized();
    //       break;
    //     }
    //   }
    //   break;

    // One of the windows must be refreshed
    case IDCMP_REFRESHWINDOW:
      GT_BeginRefresh(pMsg->IDCMPWindow);
      GT_EndRefresh(pMsg->IDCMPWindow, TRUE);
      break;
  }
}
