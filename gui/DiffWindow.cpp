#include <clib/asl_protos.h>
#include <clib/intuition_protos.h>
#include <libraries/asl.h>
#include "DiffWindow.h"


DiffWindow::DiffWindow(const struct Screen* p_pScreen)
{
  m_pScreen = p_pScreen;
  m_pWindow = NULL;
}

DiffWindow::~DiffWindow()
{
  Close();
}

bool DiffWindow::Open(DW_TYPE p_DwType)
{
  //
  // Initial validations
  //

  if(m_pScreen == NULL)
  {
    // In this design we need a screen to open the window
    return false;
  }

  if(m_pWindow != NULL)
  {
    // Not opening the window if it is already open
    return false;
  }

  //
  // Calculating window size etc in depency of screen dimesions
  //
  int screenWidth = m_pScreen->Width;
  int screenHeight = m_pScreen->Height;
  int screenBarHeight = m_pScreen->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  int activateWin = TRUE;
  int winLeft = 0;
  m_Title = "Left Diff Window";
  m_FileRequesterTitle = "Select left diff file";

  if(p_DwType == RIGHT)
  {
    winLeft = winWidth; // begin one pixel after LEFT window ends
    m_Title = "Right Diff Window";
    m_FileRequesterTitle = "Select right diff file";
    activateWin = FALSE;
  }

  //
  // Setting up the window properties
  //

  struct TagItem windowTags[] =
  {
    { WA_Left, winLeft },
    { WA_Top, screenBarHeight + 1 },
    { WA_Width, winWidth },
    { WA_Height, winHeight },
    { WA_Title, (ULONG)m_Title.C_str() },
    { WA_Activate, activateWin },
    { WA_PubScreen, (ULONG)m_pScreen },
    { WA_IDCMP, IDCMP_MENUPICK },
    { WA_NewLookMenus, TRUE },  // Ignored before v39
    { TAG_DONE, NULL },
  };

  //
  // Opening the window
  //
  m_pWindow = OpenWindowTagList(NULL, windowTags);

  return m_pWindow != NULL;
}

void DiffWindow::Close()
{
  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }
}

struct Window* DiffWindow::IntuiWindow()
{
  return m_pWindow;
}

const char* DiffWindow::Title()
{
  return m_Title.C_str();
}

void DiffWindow::SetTitle(SimpleString p_NewTitle)
{
  m_Title = p_NewTitle;

  // Call intuition function to set the window title
  // Note the ~0 inverts the value ang is a value of -1
  SetWindowTitles(m_pWindow, m_Title.C_str(), (STRPTR) ~0);
}

bool DiffWindow::Open(SimpleString p_FileName)
{
  if(p_FileName.Length() == 0)
  {
    p_FileName = aslRequestFileName();

    if(p_FileName.Length() == 0)
    {
      return false;
    }
  }

  SetTitle(p_FileName); // TODO only for test


  // TODO open the file and execute a TBD Diff command.
  // Note: this command will be have two "execute" channels
  // and will only start executing if the 2nd channel also
  // is executed.

  return true;
}

SimpleString DiffWindow::aslRequestFileName()
{
  SimpleString fileName = "";

  struct TagItem fileRequestTags[] =
  {
    ASL_Hail,       (ULONG)m_FileRequesterTitle.C_str(),
    TAG_DONE
  };

  struct TagItem requestOpeningTags[] =
  {
    ASLFR_Window, (ULONG)m_pWindow,
    TAG_DONE
  };

  struct FileRequester* pFileRequest;
  pFileRequest = (struct FileRequester*)
    AllocAslRequest(ASL_FileRequest, fileRequestTags);

  if(pFileRequest == NULL)
  {
    return fileName;
  }

  if(AslRequest(pFileRequest,requestOpeningTags) == TRUE)
  {
    STRPTR pDir = pFileRequest->rf_Dir;
    STRPTR pFile = pFileRequest->fr_File;

    fileName = pDir;
    fileName += "/";
    fileName += pFile;
  }

  FreeAslRequest(pFileRequest);

  return fileName;
}
