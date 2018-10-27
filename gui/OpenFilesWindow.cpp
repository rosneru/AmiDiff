#include <string.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>

#include "OpenFilesWindow.h"

OpenFilesWindow::OpenFilesWindow(AppScreen* p_pAppScreen,
    SimpleString& p_LeftFilePath, SimpleString& p_RightFilePath, Command& p_DiffCommand)
  : WindowBase(p_pAppScreen),
    m_LeftFilePath(p_LeftFilePath),
    m_RightFilePath(p_RightFilePath),
    m_DiffCommand(p_DiffCommand),
    m_WinWidth(240),
    m_WinHeight(180),
    m_pGadgetList(NULL),
    m_pLeftFileStringGadget(NULL),
    m_pRightFileStringGadget(NULL),
    m_pOpenLeftFileButton(NULL),
    m_pOpenRightFileButton(NULL),
    m_pDiffButton(NULL),
    m_pCancelButton(NULL)
{
  //
  // Calculate some basic values
  //
  m_FontHeight = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
  WORD barHeight = m_pAppScreen->IntuiScreen()->WBorTop + m_FontHeight + 2;

  WORD hSpace = 5;
  WORD vSpace = 10;
  WORD top = barHeight + vSpace;
  WORD left = hSpace;
  WORD right = m_WinWidth - hSpace;
  WORD bottom = m_WinHeight - vSpace;
  WORD selectButtonWidth = m_FontHeight + 2; // should be square
  WORD stringGadgetWidth = right - left - hSpace - selectButtonWidth;
  WORD selectButtonLeft = right - hSpace - selectButtonWidth;
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + 2;

  //
  // Setting up the gadgets
  //

  // Create a place for GadTools context data
  struct Gadget* pContext;
  pContext = (struct Gadget*) CreateContext(&m_pGadgetList);

  // Declare the basic gadget structure
  struct NewGadget newGadget;

  // Creating the string gadget for the file name of the left file
  newGadget.ng_TextAttr   = m_pAppScreen->GfxTextAttr();
  newGadget.ng_VisualInfo = m_pAppScreen->GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Left file";
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pLeftFileStringGadget = CreateGadget(STRING_KIND,
    pContext, &newGadget, TAG_END);

  // Creating the Button for opening a file selector for left file
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;

  m_pOpenLeftFileButton = CreateGadget(BUTTON_KIND,
    m_pLeftFileStringGadget, &newGadget, TAG_END);

  // Creating the string gadget for the file name of the right file
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    = top +  1 * (buttonHeight + vSpace);
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_GadgetID   = GID_RightFileString;

  m_pRightFileStringGadget = CreateGadget(STRING_KIND,
    m_pOpenLeftFileButton, &newGadget, TAG_END);

  // Creating the Button for opening a file selector for left file
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_TopEdge    = top +  1 * (buttonHeight + vSpace);
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;

  m_pOpenRightFileButton = CreateGadget(BUTTON_KIND,
    m_pRightFileStringGadget, &newGadget, TAG_END);

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left + hSpace;
  newGadget.ng_TopEdge    = bottom -  buttonHeight - vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pDiffButton = CreateGadget(BUTTON_KIND,
    m_pOpenRightFileButton, &newGadget, TAG_END);

  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - hSpace - buttonWidth;
  newGadget.ng_TopEdge    = bottom -  buttonHeight - vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pCancelButton = CreateGadget(BUTTON_KIND,
    m_pDiffButton, &newGadget, TAG_END);

}

OpenFilesWindow::~OpenFilesWindow()
{
  Close();
}





void OpenFilesWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool OpenFilesWindow::Open()
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
  // Calculating window size etc in dependency of screen dimensions
  //
  int screenWidth = m_pAppScreen->IntuiScreen()->Width;
  int screenHeight = m_pAppScreen->IntuiScreen()->Height;
  int screenBarHeight = m_pAppScreen->IntuiScreen()->BarHeight;

  int winLeft = screenWidth / 2 - m_WinWidth / 2;
  int winTop = screenHeight / 2 - m_WinHeight / 2;

  //
  // Opening the window
  //
  m_pWindow = OpenWindowTags(NULL,
    WA_Left, winLeft,
    WA_Top, winTop,
    WA_Width, m_WinWidth,
    WA_Height, m_WinHeight,
    WA_Title, (ULONG) "Open the files to diff",
    WA_Activate, TRUE,
    WA_PubScreen, (ULONG) m_pAppScreen->IntuiScreen(),
    WA_IDCMP,
      IDCMP_MENUPICK |      // Inform us about menu selection
      IDCMP_VANILLAKEY |    // Inform us about RAW key press
      IDCMP_RAWKEY |        // Inform us about printable key press
      IDCMP_CLOSEWINDOW |   // Inform us about click on close gadget
      IDCMP_NEWSIZE |       // Inform us about resizing
      IDCMP_REFRESHWINDOW | // Inform us when refreshing is necessary
      IDCMP_IDCMPUPDATE,    // Inform us about TODO
    WA_Flags,
      WFLG_CLOSEGADGET |    // Add a close gadget
      WFLG_DRAGBAR |        // Add a drag gadget
      WFLG_DEPTHGADGET |    // Add a depth gadget
      WFLG_SIZEGADGET |     // Add a size gadget
      WFLG_GIMMEZEROZERO,   // Different layers for border and content
    WA_SimpleRefresh, TRUE,
		WA_MinWidth, 120,
		WA_MinHeight, 90,
		WA_MaxWidth, -1,
		WA_MaxHeight, -1,
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Gadgets, m_pGadgetList,
    TAG_END);

/*
  // Setup structs for text drawing
  // TODO Remove it if not needed here
  ULONG txtPen = m_pAppScreen->IntuiDrawInfo()->dri_Pens[TEXTPEN];
  ULONG bgPen = m_pAppScreen->IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];

  m_TextAttr.ta_Name = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Flags;

  // Prepare IntuiText for line-by-line printing
  m_IntuiText.FrontPen  = txtPen;
  m_IntuiText.BackPen   = bgPen;
  m_IntuiText.DrawMode  = JAM2;
  m_IntuiText.LeftEdge  = 0;
  m_IntuiText.TopEdge   = 0;
  m_IntuiText.ITextFont = &m_TextAttr;
  m_IntuiText.NextText  = NULL;
*/

  return WindowBase::Open();
}

void OpenFilesWindow::Close()
{
  if(m_pGadgetList != NULL)
  {
    FreeGadgets(m_pGadgetList);
    m_pGadgetList = NULL;
    m_pLeftFileStringGadget = NULL;
    m_pRightFileStringGadget = NULL;
    m_pOpenLeftFileButton = NULL;
    m_pOpenRightFileButton = NULL;
    m_pDiffButton = NULL;
    m_pCancelButton = NULL;
  }

  // Also call Close() in parent
  // TODO debug if it really happens
  WindowBase::Close();
}


void OpenFilesWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(m_pWindow == 0)
  {
    return;
  }

  switch (p_Class)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) p_IAddress;
      if(pGadget->GadgetID == GID_LeftFileButton)
      {
        // TODO 1) ASL request
        //      2) Fill string gadget
        //      3) Enable Diff button if both string gadgets are filled
      }
      else if(pGadget->GadgetID == GID_RightFileButton)
      {
        // TODO 1) ASL request
        //      2) Fill string gadget
        //      3) Enable Diff button if both string gadgets are filled
      }
      else if(pGadget->GadgetID == GID_CancelButton)
      {
        Close();
      }
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
//      GT_BeginRefresh(IntuiWindow());
//      GT_EndRefresh(IntuiWindow(), TRUE);
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      Close();
      break;
    }
  }
}
