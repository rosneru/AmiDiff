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


ProgressWindow::ProgressWindow(ScreenBase*& pScreen,
                               struct MsgPort*& pIdcmpMsgPort,
                               bool& bCancelRequested,
                               AMenu* pMenu)
  : WindowBase(pScreen, pIdcmpMsgPort, pMenu),
    m_bCancelRequested(bCancelRequested),
    m_pGadtoolsContext(NULL),
    m_pGadBtnCancel(NULL),
    m_TextZero("0%"),
    m_TextHundred("100%")
{

}

ProgressWindow::~ProgressWindow()
{
  Close();

  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
    m_pGadtoolsContext = NULL;
    m_pGadBtnCancel = NULL;
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
  if(m_pScreen == NULL)
  {
    return false;
  }

  if(!WindowBase::Open(initialPos))
  {
    return false;
  }


  // Set the windows rastport to fill with a pattern for the structured
  // background
  UWORD areafillPattern[2] = {0xaaaa, 0x5555};
  m_pWindow->RPort->AreaPtrn = areafillPattern;
  m_pWindow->RPort->AreaPtSz = 1;

  struct Screen* pScreen = m_pScreen->IntuiScreen();

  // Fill the structured background
  SetDrMd(m_pWindow->RPort, JAM1);
  SetAPen(m_pWindow->RPort, m_pScreen->Pens().HighlightedText());
  RectFill(m_pWindow->RPort, 
           pScreen->WBorLeft,
           pScreen->WBorTop + pScreen->BarHeight - 1,
           m_pWindow->Width - pScreen->WBorRight - 1,
           m_pWindow->Height - pScreen->WBorBottom - 1);

  // Re-set the windows RastPort to fill without a pattern
  m_pWindow->RPort->AreaPtrn = NULL;

  // Draw the outer gray area and bevel box
  SetAPen(m_pWindow->RPort, m_pScreen->Pens().Background());
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
               GT_VisualInfo, m_pScreen->GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  // Draw a bevel box around the area where the progress bar will be
  DrawBevelBox(m_pWindow->RPort,
               m_ProgressRect.Left() - 2,
               m_ProgressRect.Top() - 1,
               m_ProgressRect.Width() + 4,
               m_ProgressRect.Height() + 3,
               GT_VisualInfo, m_pScreen->GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  // Drawing the '0%' and '100%' texts
  SetAPen(m_pWindow->RPort, m_pScreen->Pens().Text());

  Move(m_pWindow->RPort,
       (m_OuterRect.Left() + m_ProgressRect.Left()) / 2 - m_TextZeroWidth / 2,
       m_ProgressRect.Top() + m_pTextFont->tf_Baseline + 1);
  
  Text(m_pWindow->RPort, m_TextZero.C_str(), m_TextZero.Length());

  Move(m_pWindow->RPort,
       (m_OuterRect.Right() + m_ProgressRect.Right()) / 2 - m_TextHundredWidth / 2,
       m_ProgressRect.Top() + m_pTextFont->tf_Baseline + 1);
  
  Text(m_pWindow->RPort, m_TextHundred.C_str(), m_TextHundred.Length());

  // Enable the Cancel button in case it has been disabled the last
  // time the window was open
  GT_SetGadgetAttrs(m_pGadBtnCancel, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_DONE);

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
  SetAPen(m_pWindow->RPort, m_pScreen->Pens().Fill());

  // Fill the progress bar area
  RectFill(m_pWindow->RPort,
           m_ProgressRect.Left(),
           m_ProgressRect.Top(),
           m_ProgressRect.Left() + progrWidth,
           m_ProgressRect.Bottom());

  // Set color to <background> for painting the grey background of the
  // yet uncovered area of the progress bar
  SetAPen(m_pWindow->RPort, m_pScreen->Pens().Background());

  // NOTE: The following condition is a workaround because SimpleString
  // curently has no != overload
  if(!(m_ProgressDescr == pProgrMsg->pDescription))
  {
    // Update progress description as it has changed
    m_ProgressDescr = pProgrMsg->pDescription;

    // Update the description in window title
    SetWindowTitles(m_pWindow, m_ProgressDescr.C_str(), (STRPTR)~0);
  }
}

void ProgressWindow::initialize()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  m_Width = 230;
  m_Height = 78;

  struct Screen* pScreen = m_pScreen->IntuiScreen();

  size_t xOffset = 6;
  size_t yOffset = 6;
  if(m_pScreen->IsHiresMode())
  {
    // On hires reduce yOffset as the pixels there are none-square. 
    // They are double as high as wide and this is corrected here.
    yOffset /= 2;
  }

  m_OuterRect.Set(pScreen->WBorLeft + xOffset,
                  pScreen->WBorTop + pScreen->BarHeight + yOffset,
                  m_Width - pScreen->WBorRight - 1 - xOffset,
                  pScreen->WBorTop + pScreen->BarHeight + yOffset + 3 * m_pTextFont->tf_YSize);

  m_TextZeroWidth = TextLength(&pScreen->RastPort, m_TextZero.C_str(), 2);
  m_TextHundredWidth = TextLength(&pScreen->RastPort, m_TextHundred.C_str(), 4);

  m_ProgressRect.Set(m_OuterRect.Left() + m_TextZeroWidth + 2 * xOffset,
                     m_OuterRect.Top() + m_pTextFont->tf_YSize,
                     m_OuterRect.Right() - m_TextHundredWidth - 2 * xOffset, 
                     m_OuterRect.Top() + 2 * m_pTextFont->tf_YSize);


  // m_Width = (WORD)m_pScreen->IntuiScreen()->Width / 2;
  // m_FontHeight = m_pScreen->FontHeight();
  // WORD barHeight = m_pScreen->IntuiScreen()->WBorTop + m_FontHeight + 2;

  // WORD hSpace = 10;
  // WORD vSpace = 6;

  // WORD top = barHeight + vSpace;
  // WORD left = hSpace;
  // WORD right = m_Width - hSpace;
  // WORD buttonWidth = 60;
  // WORD buttonHeight = m_FontHeight + vSpace;
  // WORD stringGadgetWidth = right - left - hSpace / 2 - buttonWidth;

  // //
  // // Setting up the gadgets
  // //

  // // Create a place for GadTools context data
  // struct Gadget* pFakeGad;
  // pFakeGad = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  // if(pFakeGad == NULL)
  // {
  //   return;
  // }

  // // Setting the first gadget of the gadet list for the window
  // setFirstGadget(m_pGadtoolsContext);

  // // Declare the basic gadget structure and fill with basic values
  // struct NewGadget newGadget;
  // newGadget.ng_TextAttr   = m_pScreen->IntuiTextAttr();
  // newGadget.ng_VisualInfo = m_pScreen->GadtoolsVisualInfo();

  // // Creating the string gadget to display the progress description
  // newGadget.ng_LeftEdge   = left;
  // newGadget.ng_TopEdge    = top;
  // newGadget.ng_Width      = stringGadgetWidth;
  // newGadget.ng_Height     = buttonHeight;
  // newGadget.ng_GadgetID   = GID_TxtDescription;
  // newGadget.ng_GadgetText = NULL;
  // newGadget.ng_Flags      = 0;

  // m_pGadTxtDescription = CreateGadget(TEXT_KIND,
  //                                     pFakeGad, &newGadget,
  //                                     TAG_DONE);

  // // Preparing the newGadget struct for the progress value gadget
  // newGadget.ng_LeftEdge   = left;
  // newGadget.ng_TopEdge    += buttonHeight + vSpace;

  // // Actually the progressbar "gadget" is just a BevelBox which is
  // // drawn after window opening. So just remembering the progress
  // // gadget dimensions as they are needed later
  // m_ProgressBarLeft = newGadget.ng_LeftEdge;
  // m_ProgressBarTop = newGadget.ng_TopEdge;
  // m_ProgressBarWidth = newGadget.ng_Width;
  // m_ProgressBarHeight = newGadget.ng_Height;

  // // Creating the Cancel button in right of the "progress gadget"
  // newGadget.ng_LeftEdge   = right - buttonWidth;
  // newGadget.ng_Width      = buttonWidth;
  // newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  // newGadget.ng_GadgetID   = GID_BtnCancel;

  // m_pGadBtnCancel = CreateGadget(BUTTON_KIND,
  //                                m_pGadTxtDescription, &newGadget,
  //                                GT_Underscore, '_',
  //                                TAG_DONE);

  // // Adjust the window height depending on the y-Pos and height of the
  // // last gadget
  // m_Height = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

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

  m_bInitialized = true;
}
