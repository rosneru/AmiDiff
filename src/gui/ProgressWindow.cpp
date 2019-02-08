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
                               struct MsgPort* p_pMsgPort,
                               int& p_NumWindowsOpen,
                               bool& p_bCancelRequested)
  : WindowBase(p_AppScreen, p_pMsgPort, p_NumWindowsOpen),
    m_bCancelRequested(p_bCancelRequested),
    m_pGadgetList(NULL),
    m_pLabelDescription(NULL),
    m_pButtonCancel(NULL),
    m_ProgressBarLeft(NULL),
    m_ProgressBarTop(NULL),
    m_ProgressBarWidth(NULL),
    m_ProgressBarHeight(NULL),
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
    m_pLabelDescription = NULL;
    m_pButtonCancel = NULL;
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

  // Draw a bevel box around the area where the progress bar will be
  DrawBevelBox(m_pWindow->RPort,
    m_ProgressBarLeft, m_ProgressBarTop,
    m_ProgressBarWidth, m_ProgressBarHeight,
    GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);


  // Enable the Cancel button in case it has been disabled the last
  // time the window was open
  GT_SetGadgetAttrs(m_pButtonCancel, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

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
  WORD stringGadgetWidth = right - left - hSpace / 2 - buttonWidth;

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

  // Declare the basic gadget structure and fill with basic values
  struct NewGadget newGadget;
  newGadget.ng_TextAttr   = m_AppScreen.GfxTextAttr();
  newGadget.ng_VisualInfo = m_AppScreen.GadtoolsVisualInfo();

  // Creating the string gadget to display the progress description
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += top;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_Description;
  newGadget.ng_Flags      = 0;

  m_pLabelDescription = CreateGadget(TEXT_KIND,
    pContext, &newGadget,
    TAG_END);

  // Preparing the newGadget struct for the progress value gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_Flags      = 0;

  // But actually the "gadget" is just a BevelBox which is drawn after
  // window opening. So just remembering the progress gadget dimensions
  // as they are needed later
  m_ProgressBarLeft = newGadget.ng_LeftEdge;
  m_ProgressBarTop = newGadget.ng_TopEdge;
  m_ProgressBarWidth = newGadget.ng_Width;
  m_ProgressBarHeight = newGadget.ng_Height;

  // Creating the Cancel button in right of the "progress gadget"
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_ButtonCancel;

  m_pButtonCancel = CreateGadget(BUTTON_KIND,
    m_pLabelDescription, &newGadget, GT_Underscore, '_', TAG_END);

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_WinHeight = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  SetTitle("Comparing the files");

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
      struct Gadget* pGadget = (struct Gadget*) p_IAddress;
      if(pGadget->GadgetID == GID_ButtonCancel)
      {
        // Set the flag which will stop background process as soon as
        // possible
        m_bCancelRequested = true;

        // Disable the Cancel button
        GT_SetGadgetAttrs(m_pButtonCancel, IntuiWindow(), NULL,
          GA_Disabled, TRUE,
          TAG_END);
      }

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

  int progrWidth = 1;
  if(p_pProgressMsg->progress > 0)
  {
    progrWidth = (m_ProgressBarWidth - 2) *
      p_pProgressMsg->progress / 100;
  }

  //RectFill(m_pWindow->RPort, 10, 10, 60, 60);

/*
  printf("Fill at (%d, %d), width = %d, height = %d\n",
           m_ProgressBarLeft + 2,
           m_ProgressBarTop + 2,
           progrWidth,
           m_ProgressBarHeight - 4);
*/

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Fill());

  RectFill(m_pWindow->RPort,
           m_ProgressBarLeft + 1,
           m_ProgressBarTop + 1,
           m_ProgressBarLeft + progrWidth,
           m_ProgressBarTop + m_ProgressBarHeight - 2);

  if( p_pProgressMsg != NULL &&
     (p_pProgressMsg->pDescription != m_pProgressDescription))
  {
    m_pProgressDescription = p_pProgressMsg->pDescription;

    GT_SetGadgetAttrs(m_pLabelDescription, m_pWindow, NULL,
      GTTX_Text, m_pProgressDescription,
      TAG_END);
  }

/*
  GT_SetGadgetAttrs(m_pProgressGadget, m_pWindow, NULL,
    GTNM_Number, p_pProgressMsg->progress,
    TAG_END);

  TODO replace by DrawText
*/
}
