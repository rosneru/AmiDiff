#include <string.h>
#include <stdio.h>

#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>

#include "WorkerProgressMsg.h"
#include "ProgressWindow.h"


ProgressWindow::ProgressWindow(AppScreen& p_AppScreen,
    struct MsgPort* p_pMsgPort)
  : WindowBase(p_AppScreen, p_pMsgPort),
    m_pGadgetList(NULL),
    m_pLeftFileStringGadget(NULL),
    m_pRightFileStringGadget(NULL),
    m_pSelectLeftFileButton(NULL),
    m_pSelectRightFileButton(NULL),
    m_pDiffButton(NULL),
    m_pCancelButton(NULL),
    m_pProgressDescription(NULL)
{

}

ProgressWindow::~ProgressWindow()
{
  Close();

  if(m_pGadgetList != NULL)
  {
    FreeGadgets(m_pGadgetList);
    m_pGadgetList = NULL;
    m_pLeftFileStringGadget = NULL;
    m_pRightFileStringGadget = NULL;
    m_pSelectLeftFileButton = NULL;
    m_pSelectRightFileButton = NULL;
    m_pDiffButton = NULL;
    m_pCancelButton = NULL;
  }
}





void ProgressWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool ProgressWindow::Open(const APTR p_pUserDataMenuItemToDisable)
{
  if(WindowBase::Open(p_pUserDataMenuItemToDisable) == false)
  {
    return false;
  }

  return true;
}


void ProgressWindow::initialize()
{
  //
  // Calculate some basic values
  //
  m_WinWidth = (WORD)m_AppScreen.IntuiScreen()->Width / 2;
  m_FontHeight = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  WORD barHeight = m_AppScreen.IntuiScreen()->WBorTop + m_FontHeight + 2;

  WORD hSpace = 10;
  WORD vSpace = 10;
  WORD top = barHeight + vSpace;
  WORD left = hSpace;
  WORD right = m_WinWidth - hSpace;
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + 6;
  WORD selectButtonWidth = TextLength(
    &m_AppScreen.IntuiScreen()->RastPort, "...", 3) + 8;
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
  newGadget.ng_TextAttr   = m_AppScreen.GfxTextAttr();
  newGadget.ng_VisualInfo = m_AppScreen.GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Left file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT;

  struct Gadget* pLabelGadget = CreateGadget(TEXT_KIND,
    pContext, &newGadget, TAG_END);

  // Line 2 contains a string gadget and selection button for the
  // filename of the left file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pLeftFileStringGadget = CreateGadget(TEXT_KIND,
    pLabelGadget, &newGadget,
//    GTST_MaxChars, 200, // TODO remove constant
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;
  newGadget.ng_Flags      = 0;

  m_pSelectLeftFileButton = CreateGadget(BUTTON_KIND,
    m_pLeftFileStringGadget, &newGadget, TAG_END);

  // Line 3  contains a label
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  pLabelGadget = CreateGadget(TEXT_KIND, m_pSelectLeftFileButton,
    &newGadget, TAG_END);

  // Line 4 contains a string gadget and selection button for the
  // filename of the right file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_RightFileString;
  newGadget.ng_Flags      = 0;

  m_pRightFileStringGadget = CreateGadget(NUMBER_KIND,
    pLabelGadget, &newGadget,
    GTNM_Format, "%ld %%",
    GTNM_Border, TRUE,
    GTNM_Justification, GTJ_CENTER,
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;
  newGadget.ng_Flags      = 0;

  m_pSelectRightFileButton = CreateGadget(BUTTON_KIND,
    m_pRightFileStringGadget, &newGadget, TAG_END);

  // Line 5 conatins the buttons Diff and Cancel

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace + vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pDiffButton = CreateGadget(BUTTON_KIND,
    m_pSelectRightFileButton, &newGadget, GT_Underscore, '_', TAG_END);

  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pCancelButton = CreateGadget(BUTTON_KIND,
    m_pDiffButton, &newGadget, GT_Underscore, '_', TAG_END);

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_WinHeight = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  SetTitle("Open the files to diff");

  // Setting the window flags
  setFlags(WFLG_DRAGBAR |         // Add a drag gadget
           WFLG_DEPTHGADGET);     // Add a depth gadget

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_VANILLAKEY |     // Inform us about RAW key press
           IDCMP_RAWKEY |         // Inform us about printable key press
           IDCMP_REFRESHWINDOW |  // Inform us when refreshing is necessary
           BUTTONIDCMP);          // Inform us about GadTools button events

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadgetList);

  m_bInitialized = true;
}

bool ProgressWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(!IsOpen())
  {
    return false;
  }

  switch (p_Class)
  {
    case IDCMP_GADGETUP:
    {
      return true;
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
      GT_BeginRefresh(IntuiWindow());
      GT_EndRefresh(IntuiWindow(), TRUE);
      return true;
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      Close();
      return true;
      break;
    }
  }

  return false;
}

void ProgressWindow::HandleProgress(struct WorkerProgressMsg*
                                    p_pProgressMsg)
{
  if(p_pProgressMsg == NULL)
  {
    return;
  }

  if(p_pProgressMsg->progress < 0)
  {
    m_pProgressDescription = NULL;
    return;
  }

  RectFill(m_pWindow->RPort, 10, 10, 60, 60);

  if( p_pProgressMsg != NULL &&
     (p_pProgressMsg->pDescription != m_pProgressDescription))
  {
    m_pProgressDescription = p_pProgressMsg->pDescription;

    GT_SetGadgetAttrs(m_pLeftFileStringGadget, m_pWindow, NULL,
      GTTX_Text, m_pProgressDescription,
      TAG_END);
  }

  GT_SetGadgetAttrs(m_pRightFileStringGadget, m_pWindow, NULL,
    GTNM_Number, p_pProgressMsg->progress,
    TAG_END);
}
