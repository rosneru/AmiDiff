#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include "DiffWindow.h"



DiffWindow::DiffWindow(ScreenBase& screen,
                       const ADiffViewPens& pens,
                       struct MsgPort* pIdcmpMsgPort,
                       MenuBase* pMenu)
  : ScrollbarWindow(screen, pIdcmpMsgPort, pMenu),
    m_Pens(pens),
    m_pRPorts(NULL),
    m_pDocument(NULL),
    m_EmptyChar('\0'),
    m_pLastParentGadget(NULL),
    m_pGadtoolsContext(NULL),
    m_pGadTxtLeftFile(NULL),
    m_pGadTxtRightFile(NULL),
    m_NumDifferences(0),
    m_IndentX(5),
    m_IndentY(0),
    m_TextAreasWidth(0),
    m_TextAreasHeight(0),
    m_pTextArea1(NULL),
    m_pTextArea2(NULL)
{
  // If parent window already defined gadgets, we store the last of
  // these gadgeds and the count of defined gadgets. They are needed
  // for dynamically re-creating this window's gadgets at window
  // resizing etc.
  m_pLastParentGadget = getLastGadget();

  // Create this window's gadgets
  if(createGadgets() == false)
  {
    cleanup();
    throw "DiffWindow: Failed to create gadgets.";
  }

  // Set the default title
  SetTitle("DiffWindow");

  // Setting the window flags
  addFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DEPTHGADGET |     // Add a depth gadget
           WFLG_SIZEGADGET |      // Add a size gadget
           WFLG_GIMMEZEROZERO);   // Different layers for border and content

  // Setting the IDCMP messages we want to receive for this window
  addIDCMP(IDCMP_MENUPICK |       // Inform about menu selection
           IDCMP_CLOSEWINDOW |    // Inform about click on close gadget
           IDCMP_NEWSIZE);        // Inform about resizing
}


DiffWindow::~DiffWindow()
{
  cleanup();
}


void DiffWindow::Resized()
{
  if(!IsOpen())
  {
    return;
  }

  // TODO check if this test and m_Widht/Height assignment can be removed.
  if(m_pWindow->Width == m_Width &&
     m_pWindow->Height == m_Height)
  {
    // nothing changed
    return;
  }

  m_Width = m_pWindow->Width;
  m_Height = m_pWindow->Height;

  // Clear the window completely
  SetRast(m_pRPorts->Window(), m_Pens.Background());
  
  // Calculate some values which have to calculated after window
  // opening and after resizing
  calcSizes();

  // Resize gadgets to fit into new window size
  resizeGadgets();

  if((m_pTextArea1 == NULL) || (m_pTextArea2 == NULL))
  {
    return;
  }

  // Set location and size of the left text area
  m_pTextArea1->SetLeftTop(m_IndentX, m_IndentY);
  m_pTextArea1->SetWidthHeight(m_TextAreasWidth, m_TextAreasHeight);

  // Set location and size of the right text area
  m_pTextArea2->SetLeftTop(m_IndentX + m_TextAreasWidth, m_IndentY);
  m_pTextArea2->SetWidthHeight(m_TextAreasWidth, m_TextAreasHeight);

  // Paint the content of the two documents (from current y-position, 
  //not from start)
  m_pTextArea1->PrintFile(false);
  m_pTextArea2->PrintFile(false);

  if(m_pDocument == NULL)
  {
    return;
  }

  // Set x-scroll-gadget's pot size in relation of new window size
  setXScrollPotSize(m_pTextArea1->MaxVisibleChars(), 
                    m_pDocument->MaxLineLength());

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_pTextArea1->MaxVisibleLines(), 
                    m_pDocument->NumLines());


  // Paint the status bar
  paintStatusBar();

  // Paint the window decoration
  paintWindowDecoration();
}


bool DiffWindow::Open(InitialPosition initialPos)
{
  //
  // Open the window (and the screen if it isn't open yet)
  //
  if(!ScrollbarWindow::Open(initialPos))
  {
    return false;
  }

  if(m_pRPorts != NULL)
  {
    delete m_pRPorts;
    m_pRPorts = NULL;
  }

  m_pRPorts = new DiffWindowRastports(m_pWindow, m_Pens);
  m_pDocument = NULL;

  //
  // Calculate some initial values. It's possible that they are needed
  // in the initialize() method which is called from WindowBase::Open()
  // shortly before opening, so the Open() call is done afterwards.
  //
  m_IndentY = 2 *  m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;

  m_TextAttr.ta_Name = m_Screen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_Screen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_Screen.IntuiDrawInfo()->dri_Font->tf_Flags;


  // Calculate some sizes which are only calculatable with window
  // already open
  calcSizes();

  // With the calculated sizes the gadgets must be re-sized/positioned
  resizeGadgets();

  // Paint the window decoration
  paintWindowDecoration();

  return true;
}


bool DiffWindow::SetContent(DiffDocument* pDiffDocument)
{
  if(pDiffDocument == NULL)
  {
    return false;
  }

  m_pDocument = pDiffDocument;

  m_NumDifferences = pDiffDocument->NumAdded()
                   + pDiffDocument->NumChanged()
                   + pDiffDocument->NumDeleted();

  sprintf(m_StatusBarText,
          "Diff performed in %ld s. Total changes: %ld   |   ",
          pDiffDocument->DiffTime(),
          m_NumDifferences);

  sprintf(m_AddedText,
          "%zu Added",
          pDiffDocument->NumAdded());

  sprintf(m_ChangedText,
          "%zu Changed",
          pDiffDocument->NumChanged());

  sprintf(m_DeletedText,
          "%zu Deleted",
          pDiffDocument->NumDeleted());

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pRPorts->Window(), m_Pens.Background());

  // Display the document file names in the gadgets
  GT_SetGadgetAttrs(m_pGadTxtLeftFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pDocument->LeftFileName(),
                    TAG_DONE);

  GT_SetGadgetAttrs(m_pGadTxtRightFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pDocument->RightFileName(),
                    TAG_DONE);


  // Ensure that the gadgets are re-drawn
  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);

  calcSizes();

  // Paint the window decoration
  paintWindowDecoration();

  // Remove diff text areas if still existing
  if(m_pTextArea1 != NULL)
  {
    delete m_pTextArea1;
    m_pTextArea1 = NULL;
  }

  if(m_pTextArea2 != NULL)
  {
    delete m_pTextArea2;
    m_pTextArea2 = NULL;
  }

  m_pTextArea1 = new DiffWindowTextArea(&pDiffDocument->LeftDiffFile(), 
                                        m_pRPorts, 
                                        m_pTextFont, 
                                        pDiffDocument->LineNumbersEnabled(),
                                        pDiffDocument->MaxLineLength());

  m_pTextArea2 = new DiffWindowTextArea(&pDiffDocument->RightDiffFile(), 
                                        m_pRPorts, 
                                        m_pTextFont, 
                                        pDiffDocument->LineNumbersEnabled(),
                                        pDiffDocument->MaxLineLength());

  // Set location and size of the left text area
  m_pTextArea1->SetLeftTop(m_IndentX, m_IndentY);
  m_pTextArea1->SetWidthHeight(m_TextAreasWidth, m_TextAreasHeight);

  // Set location and size of the right text area
  m_pTextArea2->SetLeftTop(m_IndentX + m_TextAreasWidth, m_IndentY);
  m_pTextArea2->SetWidthHeight(m_TextAreasWidth, m_TextAreasHeight);

  // Paint the content of the two documents (from start)
  m_pTextArea1->PrintFile(true);
  m_pTextArea2->PrintFile(true);

  // Paint the status bar
  paintStatusBar();

  // Set x-scroll-gadget's pot size in relation of new window size
  setXScrollPotSize(m_pTextArea1->MaxVisibleChars(), 
                    m_pDocument->MaxLineLength());

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_pTextArea1->MaxVisibleLines(), 
                    m_pDocument->NumLines());

  return true;
}


void DiffWindow::NavigateToNextDiff()
{
  size_t idx = m_pDocument->NextDiffIndex();

  // Scroll y to next diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_pTextArea1->Y());
}


void DiffWindow::NavigateToPrevDiff()
{
  size_t idx = m_pDocument->PrevDiffIndex();

  // Scroll y to prev diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_pTextArea1->Y());
}


void DiffWindow::HandleIdcmp(ULONG msgClass,
                             UWORD msgCode,
                             APTR pItemAddress)
{
  ScrollbarWindow::HandleIdcmp(msgClass, msgCode, pItemAddress);

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
      handleGadgetEvent(pGadget);
      break;
    }

    case IDCMP_VANILLAKEY:
    {
      handleVanillaKey(msgCode);
      break;
    }

    case IDCMP_NEWSIZE:
    {
      Resized();
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      Close();
      break;
    }
  }
}


void DiffWindow::XChangedHandler(size_t newX)
{
  m_pTextArea1->ScrollLeftToColumn(newX);
  m_pTextArea2->ScrollLeftToColumn(newX);
}


void DiffWindow::YChangedHandler(size_t newY)
{
  m_pTextArea1->ScrollTopToRow(newY);
  m_pTextArea2->ScrollTopToRow(newY);
}


void DiffWindow::XIncrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_pTextArea1->scrollLeft(numChars);
  m_pTextArea2->scrollLeft(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollTop(m_pTextArea1->X());
  }
}


void DiffWindow::XDecrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_pTextArea1->scrollRight(numChars);
  m_pTextArea2->scrollRight(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollTop(m_pTextArea1->X());
  }
}


void DiffWindow::YIncrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_pTextArea1->scrollUp(numLines);
  m_pTextArea2->scrollUp(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_pTextArea1->Y());
  }
}


void DiffWindow::YDecrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_pTextArea1->scrollDown(numLines);
  m_pTextArea2->scrollDown(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_pTextArea1->Y());
  }
}


void DiffWindow::cleanup()
{
  if(m_pTextArea1 != NULL)
  {
    delete m_pTextArea1;
    m_pTextArea1 = NULL;
  }

  if(m_pTextArea2 != NULL)
  {
    delete m_pTextArea2;
    m_pTextArea2 = NULL;
  }

  Close();

  if(m_pRPorts != NULL)
  {
    delete m_pRPorts;
    m_pRPorts = NULL;
  }

  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
    m_pGadtoolsContext = NULL;
  }

  m_pGadTxtLeftFile = NULL;
  m_pGadTxtRightFile = NULL;
}


bool DiffWindow::createGadgets()
{
  WORD fontHeight =  m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;

  // Default location and sizes for the two string gadgets
  WORD gadWidth = m_TextAreasWidth;
  WORD gadHeight = fontHeight + 2;
  WORD gadTop = m_IndentY;
  WORD gad1Left = m_IndentX;
  WORD gad2Left = m_IndentX + 5;

  // If the text areas already exist overwrite the defaults with the actual
  if((m_pTextArea1 != NULL) && (m_pTextArea2 == NULL))
  {
    gadWidth = m_pTextArea1->Width();
    gadTop = m_pTextArea1->Top() - fontHeight - 4;
    gad1Left = m_pTextArea1->Left();
    gad2Left = m_pTextArea2->Left();
  }


  struct Gadget* pFakeGad = NULL;
  pFakeGad = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  if(pFakeGad == NULL)
  {
    return false;
  }

  if(m_pGadtoolsContext == NULL)
  {
    // This is the first call of createGadgets() in this session
    if(m_pLastParentGadget == NULL)
    {
      // Parent has no gadgets created: Set this context as first gadget
      setFirstGadget(m_pGadtoolsContext);
    }
    else
    {
      // Parent contains already gadgets: link this context as successor
      // of parents last gadget
      m_pLastParentGadget->NextGadget = m_pGadtoolsContext;
    }
  }

  struct NewGadget newGadget;
  newGadget.ng_TextAttr   = m_Screen.IntuiTextAttr();
  newGadget.ng_VisualInfo = m_Screen.GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = gad1Left;
  newGadget.ng_TopEdge    = gadTop;
  newGadget.ng_Width      = gadWidth;
  newGadget.ng_Height     = gadHeight;
  newGadget.ng_Flags      = PLACETEXT_RIGHT | NG_HIGHLABEL;
  newGadget.ng_GadgetText = NULL;

  const char* pFileName = (m_pDocument == NULL) ? &m_EmptyChar 
                                                : m_pDocument->LeftFileName();

  m_pGadTxtLeftFile = CreateGadget(TEXT_KIND,
                                   pFakeGad,
                                   &newGadget,
                                   GTTX_Border, TRUE,
                                   GTTX_Text, pFileName,
                                   TAG_DONE);
  if(m_pGadTxtLeftFile == NULL)
  {
    return false;
  }

  newGadget.ng_LeftEdge = gad2Left;
  pFileName = (m_pDocument == NULL) ? &m_EmptyChar 
                                    : m_pDocument->RightFileName();

  m_pGadTxtRightFile = CreateGadget(TEXT_KIND,
                                    m_pGadTxtLeftFile,
                                    &newGadget,
                                    GTTX_Border, TRUE,
                                    GTTX_Text, pFileName,
                                    TAG_DONE);
  if(m_pGadTxtRightFile == NULL)
  {
    return false;
  }

  return true;
}


void DiffWindow::handleGadgetEvent(struct Gadget* pGadget)
{
  if(pGadget == NULL)
  {
    return;
  }

  // Currently nothing to handle
}


void DiffWindow::handleVanillaKey(UWORD code)
{
  // Currently nothing to handle
}


void DiffWindow::calcSizes()
{
  // Calculate the current m_InnerWindowRight and m_InnerWindowBottom
  ScrollbarWindow::calcSizes();

  // Calculate the 
  m_TextAreasWidth = m_InnerWindowRight - m_IndentX - m_IndentX;
  m_TextAreasWidth /= 2;

  // Pre-calc text areas heigt. Will later be limited to int multiples.
  m_TextAreasHeight = m_InnerWindowBottom - m_IndentY - m_IndentY;
}


void DiffWindow::resizeGadgets()
{
  // Save a copy of the soon to be obsolete GadgTools context
  struct Gadget* pOldContext = m_pGadtoolsContext;

  // Detach this windows gadgets from the ones defined in parent window
  m_pLastParentGadget->NextGadget = NULL;

  // Remove these gadgets from the windows gadget list
  RemoveGList(m_pWindow, m_pGadtoolsContext, -1);

  // Clear the area on which the new gadgets will be drawn
  RectFill(m_pRPorts->APenBackgr(),
           0,
           0,
           m_InnerWindowRight,
           m_pTextArea1->Top() - 3);

  // Re-create the gadgets with new position and size
  // TODO handle failure.
  createGadgets();

  AddGList(m_pWindow, m_pGadtoolsContext, (UWORD)~0, -1, NULL);
  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);
  FreeGadgets(pOldContext);

  if(m_pDocument == NULL)
  {
    return;
  }

  // Display the document file names in the gadgets
  GT_SetGadgetAttrs(m_pGadTxtLeftFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pDocument->LeftFileName(),
                    TAG_DONE);

  GT_SetGadgetAttrs(m_pGadTxtRightFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pDocument->RightFileName(),
                    TAG_DONE);
}


void DiffWindow::paintWindowDecoration()
{
  if((m_pTextArea1 == NULL) || (m_pTextArea2 == NULL))
  {
    return;
  }

  // Create borders for the two text areas
  DrawBevelBox(m_pRPorts->Window(),
               m_pTextArea1->Left(),
               m_pTextArea1->Top(),
               m_pTextArea1->Width(),
               m_pTextArea1->Height(),
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  DrawBevelBox(m_pRPorts->Window(),
               m_pTextArea2->Left(),
               m_pTextArea2->Top(),
               m_pTextArea2->Width(),
               m_pTextArea2->Height(),
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);
}


void DiffWindow::paintStatusBar()
{
  if(!IsOpen() || (m_pTextArea1 == NULL))
  {
    return;
  }

  int top = m_pTextArea1->Top() + m_pTextArea1->Height() + m_InnerWindowBottom;
  top /= 2;
  top -= m_Screen.IntuiDrawInfo()->dri_Font->tf_Baseline;
  top++;

  int left = m_pTextArea1->Left() + 2;

  // Clear the status bar area
  RectFill(m_pRPorts->APenBackgr(),
           0,
           top,
           m_InnerWindowRight,
           m_InnerWindowBottom);


  struct IntuiText intuiText;
  intuiText.FrontPen  = m_Pens.Text();
  intuiText.BackPen   = m_Pens.Background();
  intuiText.DrawMode  = JAM2;
  intuiText.ITextFont = &m_TextAttr;
  intuiText.NextText  = NULL;

  intuiText.TopEdge   = top;
  intuiText.LeftEdge  = left;
  intuiText.IText = (UBYTE*) m_StatusBarText;
  PrintIText(m_pRPorts->Window(), &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText);
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Pens.Green();
  intuiText.IText = (UBYTE*) m_AddedText;
  PrintIText(m_pRPorts->Window(), &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Pens.Yellow();
  intuiText.IText = (UBYTE*) m_ChangedText;
  PrintIText(m_pRPorts->Window(), &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Pens.Red();
  intuiText.IText = (UBYTE*) m_DeletedText;
  PrintIText(m_pRPorts->Window(), &intuiText, 0, 0);
}
