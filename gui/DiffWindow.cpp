#include <string.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <libraries/asl.h>
#include <libraries/dos.h>

#include "DiffWindow.h"


DiffWindow::DiffWindow(AppScreen* p_pAppScreen)
  : m_pAppScreen(p_pAppScreen),
    m_pWindow(NULL),
    m_Y(0)
{
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

  if(m_pAppScreen == NULL)
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
  int screenWidth = m_pAppScreen->IntuiScreen()->Width;
  int screenHeight = m_pAppScreen->IntuiScreen()->Height;
  int screenBarHeight = m_pAppScreen->IntuiScreen()->BarHeight;

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
    { WA_PubScreen, (ULONG)m_pAppScreen->IntuiScreen() },
    { WA_IDCMP, IDCMP_MENUPICK | IDCMP_VANILLAKEY | IDCMP_RAWKEY},
    { WA_NewLookMenus, TRUE },  // Ignored before v39
    { WA_Flags, WFLG_GIMMEZEROZERO },
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

struct Window* DiffWindow::IntuiWindow()
{
  return m_pWindow;
}

bool DiffWindow::ReadFile(SimpleString p_FileName)
{
  if(p_FileName.Length() == 0)
  {
    p_FileName = aslRequestFileName();

    if(p_FileName.Length() == 0)
    {
      return false;
    }
  }

  if(DiffDocument::ReadFile(p_FileName) == false)
  {
    return false;
  }

  // Set full path of opened file as window title
  SetTitle(p_FileName);
  displayFile();
  return true;
}

void DiffWindow::displayFile()
{
  // Setup Pens, TextAttr and prepare IntuiText
  // TODO Remove it to some better place
  struct DrawInfo* pDrawInfo = m_pAppScreen->IntuiDrawInfo();
  ULONG txtPen = pDrawInfo->dri_Pens[TEXTPEN];
  ULONG bgPen = pDrawInfo->dri_Pens[BACKGROUNDPEN];

  struct TextAttr textAttr;
  textAttr.ta_Name = pDrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
  textAttr.ta_YSize = pDrawInfo->dri_Font->tf_YSize;
  textAttr.ta_Style = pDrawInfo->dri_Font->tf_Style;
  textAttr.ta_Flags = pDrawInfo->dri_Font->tf_Flags;

  // Prepare IntuiText for line-by-line printing
  struct IntuiText intuiText;
  intuiText.FrontPen  = txtPen;
  intuiText.BackPen   = bgPen;
  intuiText.DrawMode  = JAM2;
  intuiText.LeftEdge  = 0;
  intuiText.TopEdge   = 0;
  intuiText.ITextFont = &textAttr;
  intuiText.NextText  = NULL;

  SimpleString* pLine = GetFirstLine();
  while(pLine != NULL)
  {
    // Output the line in the window
    intuiText.IText = (UBYTE*)pLine->C_str();
    PrintIText(m_pWindow->RPort, &intuiText, 10, 10);

    // Increment Y value of struct IntuiText in preparation of the next
    // line
    intuiText.TopEdge += textAttr.ta_YSize;

    pLine = GetNextLine();
  }
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

  if(AslRequest(pFileRequest,requestOpeningTags))
  {
    // Copying path name into a big enough buffer
    char fullPathBuf[512];
    strcpy(fullPathBuf, pFileRequest->rf_Dir);

    // Calling a dos.library function to combine path and file name
    if(AddPart(fullPathBuf, pFileRequest->fr_File, 512))
    {
     fileName = fullPathBuf;
    }
  }

  FreeAslRequest(pFileRequest);

  return fileName;
}
