#include <string.h>
#include <stdio.h>

#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>

#include "ProgressMessage.h"
#include "ProgressWindow.h"


ProgressWindow::ProgressWindow(AppScreen& appScreen,
                               struct MsgPort*& pIdcmpMsgPort,
                               int& numOpenWindows,
                               bool& bCancelRequested)
  : WindowBase(appScreen, pIdcmpMsgPort, numOpenWindows),
    m_bCancelRequested(bCancelRequested),
    m_pGadtoolsContext(NULL),
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

  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
    m_pGadtoolsContext = NULL;
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

bool ProgressWindow::Open(const APTR pMenuItemDisableAtOpen,
                          InitialPosition initialPos)
{
  if(!WindowBase::Open(pMenuItemDisableAtOpen, initialPos))
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

  // Initialize the intui text structure for progress value display
  m_ProgressValueIText.FrontPen  = m_AppScreen.Pens().HighlightedText();
  m_ProgressValueIText.BackPen   = m_AppScreen.Pens().Background();
  m_ProgressValueIText.DrawMode  = JAM1;
  m_ProgressValueIText.NextText  = NULL;
  m_ProgressValueIText.LeftEdge  = m_ProgressBarLeft + 3;
  m_ProgressValueIText.TopEdge   = m_ProgressBarTop + 3;

  return true;
}


bool ProgressWindow::HandleIdcmp(ULONG msgClass,
                                 UWORD msgCode,
                                 APTR pItemAddress)
{
  if(!IsOpen())
  {
    return false;
  }

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
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


void ProgressWindow::HandleProgress(struct ProgressMessage* pProgrMsg)
{
  if(pProgrMsg == NULL)
  {
    return;
  }

  if(pProgrMsg->progress < 0)
  {
    m_pProgressDescription = NULL;
    return;
  }

  int progrWidth = 1;
  if(pProgrMsg->progress > 0)
  {
    progrWidth = (m_ProgressBarWidth - 2) *
      pProgrMsg->progress / 100;
  }

  // Set color to <blue> for painting the progress bar
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Fill());

  // Fill the progress bar area
  RectFill(m_pWindow->RPort,
           m_ProgressBarLeft + 2,
           m_ProgressBarTop + 1,
           m_ProgressBarLeft + progrWidth - 1,
           m_ProgressBarTop + m_ProgressBarHeight - 2);

  // Set color to <background> for painting the grey background of the
  // yet uncovered area of the progress bar
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Fill the yet uncovered progress bar area with the background
  // color. This is necessary to clear the formerly printed text.
  RectFill(m_pWindow->RPort,
           m_ProgressBarLeft + 2 + progrWidth,
           m_ProgressBarTop + 1,
           m_ProgressBarLeft + m_ProgressBarWidth - 3,
           m_ProgressBarTop + m_ProgressBarHeight - 2);


  if( pProgrMsg != NULL &&
     (pProgrMsg->pDescription != m_pProgressDescription))
  {
    m_pProgressDescription = pProgrMsg->pDescription;

    GT_SetGadgetAttrs(m_pLabelDescription, m_pWindow, NULL,
      GTTX_Text, m_pProgressDescription,
      TAG_END);
  }

  SimpleString progrText = pProgrMsg->progress;
  progrText += " %";

  m_ProgressValueIText.IText = (UBYTE*) progrText.C_str();

  int textLength = IntuiTextLength(&m_ProgressValueIText);
  int x = (m_ProgressBarWidth - textLength) / 2;

  PrintIText(m_pWindow->RPort, &m_ProgressValueIText, x, 0);
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
  struct Gadget* pFakeGad;
  pFakeGad = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  if(pFakeGad == NULL)
  {
    return;
  }

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadtoolsContext);

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
                                     pFakeGad, &newGadget,
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
                                 m_pLabelDescription, &newGadget,
                                 GT_Underscore, '_',
                                 TAG_END);

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

  SetSmartRefresh(true);

  m_bInitialized = true;
}
