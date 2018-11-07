#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>

#include "OpenFilesWindow.h"

OpenFilesWindow::OpenFilesWindow(AppScreen* p_pAppScreen,
    struct MsgPort* p_pMsgPort, AmigaDiffFacade& p_DiffFacade)
  : WindowBase(p_pAppScreen, p_pMsgPort),
    m_DiffFacade(p_DiffFacade),
    m_WinWidth((WORD)p_pAppScreen->IntuiScreen()->Width / 2),
    m_WinHeight(120),
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

  WORD hSpace = 10;
  WORD vSpace = 10;
  WORD top = barHeight + vSpace;
  WORD left = hSpace;
  WORD right = m_WinWidth - hSpace;
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + 6;
  WORD selectButtonWidth = TextLength(
    &m_pAppScreen->IntuiScreen()->RastPort, "...", 3) + 8;
  WORD stringGadgetWidth = right - left - hSpace / 2 - selectButtonWidth;
  WORD selectButtonLeft = right - selectButtonWidth;

  //
  // Setting up the gadgets
  //

  // Create a place for GadTools context data
  struct Gadget* pContext;
  pContext = (struct Gadget*) CreateContext(&m_pGadgetList);
  if(pContext == NULL)
  {
    return;
  }

  // Declare the basic gadget structure
  struct NewGadget newGadget;

  // Line 1  contains  a label
  newGadget.ng_TextAttr   = m_pAppScreen->GfxTextAttr();
  newGadget.ng_VisualInfo = m_pAppScreen->GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Left file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  struct Gadget* pLabelGadget = CreateGadget(TEXT_KIND,
    pContext, &newGadget, TAG_END);

  // Line 2 contains a string gadget and selection button for the
  // filename of the left file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Left file";
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pLeftFileStringGadget = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget, TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;

  m_pOpenLeftFileButton = CreateGadget(BUTTON_KIND,
    m_pLeftFileStringGadget, &newGadget, TAG_END);

  // Line 3  contains a label
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  pLabelGadget = CreateGadget(TEXT_KIND, m_pOpenLeftFileButton,
    &newGadget, TAG_END);

  // Line 4 contains a string gadget and selection button for the
  // filename of the right file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_GadgetID   = GID_RightFileString;
  newGadget.ng_Flags      = 0;

  m_pRightFileStringGadget = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget, TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;

  m_pOpenRightFileButton = CreateGadget(BUTTON_KIND,
    m_pRightFileStringGadget, &newGadget, TAG_END);

  // Line 5 conatins the buttons Diff and Cancel

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace + vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pDiffButton = CreateGadget(BUTTON_KIND,
    m_pOpenRightFileButton, &newGadget, GT_Underscore, '_', TAG_END);

  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pCancelButton = CreateGadget(BUTTON_KIND,
    m_pDiffButton, &newGadget, GT_Underscore, '_', TAG_END);

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_WinHeight = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;
}

OpenFilesWindow::~OpenFilesWindow()
{
  Close();

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
}





void OpenFilesWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool OpenFilesWindow::Open(APTR p_pUserDataMenuItemToDisable)
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
    WA_Flags,
      WFLG_CLOSEGADGET |    // Add a close gadget
      WFLG_DRAGBAR |        // Add a drag gadget
      WFLG_DEPTHGADGET,     // Add a depth gadget
    WA_SimpleRefresh, TRUE,
		WA_MinWidth, 120,
		WA_MinHeight, 90,
		WA_MaxWidth, -1,
		WA_MaxHeight, -1,
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Gadgets, m_pGadgetList,
    TAG_END);

  if(m_pWindow == NULL)
  {
    return false;
  }

  // The window should be using this message port which might be shared
  // with other windows
  m_pWindow->UserPort = m_pMsgPort;

  // Setting up the window's IDCMP flags
  ULONG flags = IDCMP_MENUPICK |      // Inform us about menu selection
                IDCMP_VANILLAKEY |    // Inform us about RAW key press
                IDCMP_RAWKEY |        // Inform us about printable key press
                IDCMP_CLOSEWINDOW |   // Inform us about click on close gadget
                IDCMP_REFRESHWINDOW | // Inform us when refreshing is necessary
                BUTTONIDCMP;          // Inform us about GadTools button events

  ModifyIDCMP(m_pWindow, flags);

  // Set the Diff button to an initial enabled / disabled state
  setDiffButtonState();


  return WindowBase::Open(p_pUserDataMenuItemToDisable);
}

void OpenFilesWindow::Close()
{
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
      else if(pGadget->GadgetID == GID_LeftFileString)
      {
        // Set the changed string gadget text as left file path
        UBYTE* pBuf = ((struct StringInfo*)
          m_pLeftFileStringGadget->SpecialInfo)->Buffer;

        m_DiffFacade.SetLeftFilePath((const char*)pBuf);

        setDiffButtonState();
      }
      else if(pGadget->GadgetID == GID_RightFileString)
      {
        // Set the changed string gadget text as right file path
        UBYTE* pBuf = ((struct StringInfo*)
          m_pRightFileStringGadget->SpecialInfo)->Buffer;

        m_DiffFacade.SetRightFilePath((const char*)pBuf);

        setDiffButtonState();
      }

      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
      GT_BeginRefresh(IntuiWindow());
      GT_EndRefresh(IntuiWindow(), TRUE);
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      Close();
      break;
    }
  }
}


void OpenFilesWindow::setDiffButtonState()
{
  if(m_pWindow == NULL || m_pDiffButton == NULL)
  {
    return;
  }

  if(m_DiffFacade.LeftFilePath().Length() > 0 &&
     m_DiffFacade.RightFilePath().Length() > 0)
  {
    // Enable "Diff" button
    GT_SetGadgetAttrs(m_pDiffButton, m_pWindow, NULL,
      GA_Disabled, FALSE,
      TAG_END);
  }
  else
  {
    // Disable "Diff" button
    GT_SetGadgetAttrs(m_pDiffButton, m_pWindow, NULL,
      GA_Disabled, TRUE,
      TAG_END);

  }
}
