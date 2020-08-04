#include <string.h>
#include <stdio.h>

#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>

#include "ProgressMessage.h"
#include "ProgressWindow.h"


ProgressWindow::ProgressWindow(ScreenBase& screen,
                               struct MsgPort* pIdcmpMsgPort,
                               bool& isCancelRequested,
                               AMenu* pMenu)
  : WindowBase(screen, pIdcmpMsgPort, pMenu),
    m_IsCancelRequested(isCancelRequested),
    m_NewGadget(),
    m_pGadtoolsContext(NULL),
    m_pGadBtnStop(NULL),
    m_TextZero("0%"),
    m_TextHundred("100%")
{

  m_Width = 230;

  struct Screen* pScr = m_Screen.IntuiScreen();

  size_t xOffset = 6;
  size_t yOffset = 6;
  if(m_Screen.IsHiresMode())
  {
    // When in hires mode the pixels are none-square. They are twice
    // as high as wide, and this is corrected here.
    yOffset /= 2;
  }

  m_OuterRect.Set(pScr->WBorLeft + xOffset,
                  pScr->WBorTop + pScr->BarHeight + yOffset,
                  m_Width - pScr->WBorRight - 1 - xOffset,
                  pScr->WBorTop + pScr->BarHeight + yOffset + 3 * m_pTextFont->tf_YSize);

  m_TextZeroWidth = TextLength(&pScr->RastPort, m_TextZero.c_str(), 2);
  m_TextHundredWidth = TextLength(&pScr->RastPort, m_TextHundred.c_str(), 4);

  m_ProgressRect.Set(m_OuterRect.Left() + m_TextZeroWidth + 2 * xOffset,
                     m_OuterRect.Top() + m_pTextFont->tf_YSize,
                     m_OuterRect.Right() - m_TextHundredWidth - 2 * xOffset, 
                     m_OuterRect.Top() + 2 * m_pTextFont->tf_YSize);

  //
  // Setting up the gadgets
  //

  // Create GadTools context data
  m_pGadtoolsContext = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  if(m_pGadtoolsContext == NULL)
  {
    return;
  }

  size_t buttonWidth = 60;

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadtoolsContext);

  // Declare the basic gadget structure and fill with basic values
  m_NewGadget.ng_TextAttr   = m_Screen.IntuiTextAttr();
  m_NewGadget.ng_VisualInfo = m_Screen.GadtoolsVisualInfo();

  // Creating the string gadget to display the progress description
  m_NewGadget.ng_LeftEdge   = m_Width / 2 - buttonWidth / 2;
  m_NewGadget.ng_TopEdge    = m_OuterRect.Bottom() + yOffset;
  m_NewGadget.ng_Width      = buttonWidth;
  m_NewGadget.ng_Height     = m_pTextFont->tf_YSize + 2 * yOffset;
  m_NewGadget.ng_GadgetID   = GID_BtnStop;
  m_NewGadget.ng_GadgetText = (UBYTE*) "Stop";
  m_NewGadget.ng_Flags      = 0;

  m_Height = m_NewGadget.ng_TopEdge + m_NewGadget.ng_Height + yOffset + pScr->WBorBottom;

  // Setting window title
  SetTitle("Progress window");

  // Setting the window flags
  addFlags(WFLG_DRAGBAR |         // Add a drag gadget
           WFLG_DEPTHGADGET);     // Add a depth gadget


  // Setting the IDCMP messages we want to receive for this window
  addIDCMP(IDCMP_MENUPICK |       // Get msg when menu selected
           IDCMP_VANILLAKEY |     // Get msg when RAW key pressed
           IDCMP_RAWKEY |         // Get msg when printable key pressed
           IDCMP_REFRESHWINDOW |  // Get msg when must refreshing
           IDCMP_GADGETUP);       // Get msg when gadgets changed
}

ProgressWindow::~ProgressWindow()
{
  Close();

  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
    m_pGadtoolsContext = NULL;
    m_pGadBtnStop = NULL;
  }
}





void ProgressWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool ProgressWindow::Open(InitialPosition initialPos)
{
  if(!WindowBase::Open(initialPos))
  {
    return false;
  }


  // Set the windows rastport to fill with a pattern for the structured
  // background
  UWORD areafillPattern[2] = {0xaaaa, 0x5555};
  m_pWindow->RPort->AreaPtrn = areafillPattern;
  m_pWindow->RPort->AreaPtSz = 1;

  struct Screen* pScreen = m_Screen.IntuiScreen();

  // Fill the structured background
  SetDrMd(m_pWindow->RPort, JAM1);
  SetAPen(m_pWindow->RPort, m_Screen.Pens().HighlightedText());
  RectFill(m_pWindow->RPort, 
           pScreen->WBorLeft,
           pScreen->WBorTop + pScreen->BarHeight - 1,
           m_pWindow->Width - pScreen->WBorRight - 1,
           m_pWindow->Height - pScreen->WBorBottom - 1);

  // Re-set the windows RastPort to fill without a pattern
  m_pWindow->RPort->AreaPtrn = NULL;

  // Draw the outer gray area and bevel box
  SetAPen(m_pWindow->RPort, m_Screen.Pens().Background());
  RectFill(m_pWindow->RPort, 
           m_OuterRect.Left(),
           m_OuterRect.Top(),
           m_OuterRect.Right() - 1,
           m_OuterRect.Bottom() - 1);

  DrawBevelBox(m_pWindow->RPort,
               m_OuterRect.Left(),
               m_OuterRect.Top(),
               m_OuterRect.Width(),
               m_OuterRect.Height(),
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  // Draw a bevel box around the area where the progress bar will be
  DrawBevelBox(m_pWindow->RPort,
               m_ProgressRect.Left() - 2,
               m_ProgressRect.Top() - 1,
               m_ProgressRect.Width() + 4,
               m_ProgressRect.Height() + 3,
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  // Drawing the '0%' and '100%' texts
  SetAPen(m_pWindow->RPort, m_Screen.Pens().Text());

  Move(m_pWindow->RPort,
       (m_OuterRect.Left() + m_ProgressRect.Left()) / 2 - m_TextZeroWidth / 2,
       m_ProgressRect.Top() + m_pTextFont->tf_Baseline + 1);
  
  Text(m_pWindow->RPort, m_TextZero.c_str(), m_TextZero.length());

  Move(m_pWindow->RPort,
       (m_OuterRect.Right() + m_ProgressRect.Right()) / 2 - m_TextHundredWidth / 2,
       m_ProgressRect.Top() + m_pTextFont->tf_Baseline + 1);
  
  Text(m_pWindow->RPort, m_TextHundred.c_str(), m_TextHundred.length());

  // Create the stop gadget
  m_pGadBtnStop = CreateGadget(BUTTON_KIND,
                               m_pGadtoolsContext, &m_NewGadget,
                               TAG_DONE);

  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);

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
      if(pGadget->GadgetID == GID_BtnStop)
      {
        // Set the flag which will stop background process as soon as
        // possible
        m_IsCancelRequested = true;

        // Disable the Cancel button
        GT_SetGadgetAttrs(m_pGadBtnStop, IntuiWindow(), NULL,
          GA_Disabled, TRUE,
          TAG_DONE);
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
    progrWidth = (m_ProgressRect.Width() - 2) * pProgrMsg->progress / 100;
  }

  // Set color to <blue> for painting the progress bar
  SetAPen(m_pWindow->RPort, m_Screen.Pens().Fill());

  // Fill the progress bar area
  RectFill(m_pWindow->RPort,
           m_ProgressRect.Left(),
           m_ProgressRect.Top(),
           m_ProgressRect.Left() + progrWidth,
           m_ProgressRect.Bottom());

  // Set color to <background> for painting the grey background of the
  // yet uncovered area of the progress bar
  SetAPen(m_pWindow->RPort, m_Screen.Pens().Background());

  // NOTE: The following condition is a workaround because std::string
  // curently has no != overload
  if(!(m_ProgressDescr == pProgrMsg->pDescription))
  {
    // Update progress description as it has changed
    m_ProgressDescr = pProgrMsg->pDescription;

    // Update the description in window title
    SetWindowTitles(m_pWindow, m_ProgressDescr.c_str(), (STRPTR)~0);
  }
}
