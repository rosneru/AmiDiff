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


ProgressWindow::ProgressWindow(AScreen& appScreen,
                               struct MsgPort*& pIdcmpMsgPort,
                               int& numOpenWindows,
                               bool& bCancelRequested)
  : WindowBase(appScreen, pIdcmpMsgPort, numOpenWindows),
    m_bCancelRequested(bCancelRequested),
    m_pGadtoolsContext(NULL),
    m_pGadTxtDescription(NULL),
    m_pGadBtnCancel(NULL),
    m_ProgressBarLeft(0),
    m_ProgressBarTop(0),
    m_ProgressBarWidth(0),
    m_ProgressBarHeight(0),
    m_ProgressValueIText()
{

}

ProgressWindow::~ProgressWindow()
{
  Close();

  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
    m_pGadtoolsContext = NULL;
    m_pGadTxtDescription = NULL;
    m_pGadBtnCancel = NULL;
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
    GT_VisualInfo, m_AScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);


  // Enable the Cancel button in case it has been disabled the last
  // time the window was open
  GT_SetGadgetAttrs(m_pGadBtnCancel, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  return true;
}


void ProgressWindow::HandleIdcmp(ULONG msgClass,
                                 UWORD msgCode,
                                 APTR pItemAddress)
{
  if(!IsOpen())
  {
    return;
  }

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
      if(pGadget->GadgetID == GID_BtnCancel)
      {
        // Set the flag which will stop background process as soon as
        // possible
        m_bCancelRequested = true;

        // Disable the Cancel button
        GT_SetGadgetAttrs(m_pGadBtnCancel, IntuiWindow(), NULL,
          GA_Disabled, TRUE,
          TAG_END);
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
  }
}


void ProgressWindow::HandleProgress(struct ProgressMessage* pProgrMsg)
{
  if(!IsOpen())
  {
    return;
  }

  if(pProgrMsg == NULL)
  {
    return;
  }

  if(pProgrMsg->progress < 0)
  {
    m_ProgressDescr = "";
    return;
  }

  int progrWidth = 1;
  if(pProgrMsg->progress > 0)
  {
    progrWidth = (m_ProgressBarWidth - 2) * pProgrMsg->progress / 100;
  }

  // Set color to <blue> for painting the progress bar
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Fill());

  // Fill the progress bar area
  RectFill(m_pWindow->RPort,
           m_ProgressBarLeft + 2,
           m_ProgressBarTop + 1,
           m_ProgressBarLeft + progrWidth - 1,
           m_ProgressBarTop + m_ProgressBarHeight - 2);

  // Set color to <background> for painting the grey background of the
  // yet uncovered area of the progress bar
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Background());

  // Fill the yet uncovered progress bar area with the background
  // color. This is necessary to clear the formerly printed text.
  RectFill(m_pWindow->RPort,
           m_ProgressBarLeft + 2 + progrWidth,
           m_ProgressBarTop + 1,
           m_ProgressBarLeft + m_ProgressBarWidth - 3,
           m_ProgressBarTop + m_ProgressBarHeight - 2);

  // NOTE: The following condition is a workaround because SimpleString
  // curently has no != overload
  if(!(m_ProgressDescr == pProgrMsg->pDescription))
  {
    // Progress description as it has changed
    m_ProgressDescr = pProgrMsg->pDescription;

    // Update the description gadged
    GT_SetGadgetAttrs(m_pGadTxtDescription,
                      m_pWindow,
                      NULL,
                      GTTX_Text, m_ProgressDescr.C_str(),
                      TAG_DONE);
  }

  m_ProgressValue = pProgrMsg->progress;
  m_ProgressValue += " %";

  m_ProgressValueIText.IText = (UBYTE*) m_ProgressValue.C_str();

  // Calculate dX to print the text centered
  LONG textLength = IntuiTextLength(&m_ProgressValueIText);
  ULONG dX = textLength / 2;


  PrintIText(m_pWindow->RPort, &m_ProgressValueIText, -dX, 0);
}


void ProgressWindow::initialize()
{
  m_WinWidth = (WORD)m_AScreen.IntuiScreen()->Width / 2;
  m_FontHeight = m_AScreen.FontHeight();
  WORD barHeight = m_AScreen.IntuiScreen()->WBorTop + m_FontHeight + 2;

  WORD hSpace = 10;
  WORD vSpace = 6;

  WORD top = barHeight + vSpace;
  WORD left = hSpace;
  WORD right = m_WinWidth - hSpace;
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + vSpace;
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
  newGadget.ng_TextAttr   = m_AScreen.IntuiTextAttr();
  newGadget.ng_VisualInfo = m_AScreen.GadtoolsVisualInfo();

  // Creating the string gadget to display the progress description
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetID   = GID_TxtDescription;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_Flags      = 0;

  m_pGadTxtDescription = CreateGadget(TEXT_KIND,
                                      pFakeGad, &newGadget,
                                      TAG_END);

  // Preparing the newGadget struct for the progress value gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;

  // Actually the progressbar "gadget" is just a BevelBox which is
  // drawn after window opening. So just remembering the progress
  // gadget dimensions as they are needed later
  m_ProgressBarLeft = newGadget.ng_LeftEdge;
  m_ProgressBarTop = newGadget.ng_TopEdge;
  m_ProgressBarWidth = newGadget.ng_Width;
  m_ProgressBarHeight = newGadget.ng_Height;

  // Creating the Cancel button in right of the "progress gadget"
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_BtnCancel;

  m_pGadBtnCancel = CreateGadget(BUTTON_KIND,
                                 m_pGadTxtDescription, &newGadget,
                                 GT_Underscore, '_',
                                 TAG_END);

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_WinHeight = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  SetTitle("ADiffView progress..");

  // Setting the window flags
  setFlags(WFLG_DRAGBAR |         // Add a drag gadget
           WFLG_DEPTHGADGET);     // Add a depth gadget


  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Get msg when menu selected
           IDCMP_VANILLAKEY |     // Get msg when RAW key pressed
           IDCMP_RAWKEY |         // Get msg when printable key pressed
           IDCMP_REFRESHWINDOW |  // Get msg when must refreshing
           IDCMP_GADGETUP);       // Get msg when gadgets changed

  SetSmartRefresh(true);

  // Initialize the intui text structure for progress value display
  m_ProgressValueIText.FrontPen  = m_AScreen.Pens().HighlightedText();
  m_ProgressValueIText.BackPen   = m_AScreen.Pens().Background();
  m_ProgressValueIText.DrawMode  = JAM1;
  m_ProgressValueIText.LeftEdge  = (m_ProgressBarWidth / 2) + m_ProgressBarLeft;

  m_ProgressValueIText.TopEdge   = m_ProgressBarTop + ((m_ProgressBarHeight - m_FontHeight) / 2);

  m_ProgressValueIText.ITextFont = NULL;
  m_ProgressValueIText.NextText  = NULL;


  m_bInitialized = true;
}
