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
    m_TextArea1(m_pRPorts),
    m_TextArea2(m_pRPorts)
{
  // If parent window already defined gadgets, we store the last of
  // these gadgeds and the count of defined gadgets. They are needed
  // for dynamically re-creating this window's gadgets at window
  // resizing etc.
  m_pLastParentGadget = getLastGadget();

  // Create this window's gadgets
  createGadgets();

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


void DiffWindow::Resized()
{
  if(!IsOpen())
  {
    return;
  }

  if(m_pWindow->Width == m_Width &&
     m_pWindow->Height == m_Height)
  {
    // nothing changed
    return;
  }

  m_Width = m_pWindow->Width;
  m_Height = m_pWindow->Height;

  // Calculate some values which have to calculated after window
  // opening and after resizing
  calcSizes();

  // Clear the window completely
  SetRast(m_pRPorts->Window(), m_Pens.Background());

  // Resize gadgets to fit into new window size
  resizeGadgets();

  if(m_pDocument != NULL)
  {
    // Paint the content of the two documents
    m_TextArea1.paintDocuments(false);
    m_TextArea2.paintDocuments(false);
  }

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
  m_TextArea1.SetLeftTop(m_IndentX, m_IndentY);

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

  // Set document content to the text areas
  
  // TODO Change to new TextArea1()
  m_TextArea1.SetDocument(&pDiffDocument->LeftDiffFile(), 
                          m_pTextFont, 
                          pDiffDocument->MaxLineLength(),
                          pDiffDocument->LineNumbersEnabled());

  m_TextArea2.SetDocument(&pDiffDocument->RightDiffFile(), 
                          m_pTextFont, 
                          pDiffDocument->MaxLineLength(),
                          pDiffDocument->LineNumbersEnabled());

  // Paint the status bar
  paintStatusBar();

  setXScrollPotSize(m_TextArea1.NumVisibleChars(), 
                    m_pDocument->MaxLineLength());

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_TextArea1.NumVisibleLines(), 
                    m_pDocument->NumLines());

  return true;
}


void DiffWindow::NavigateToNextDiff()
{
  size_t idx = m_pDocument->NextDiffIndex();

  // Scroll y to next diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_TextArea1.Y());
}


void DiffWindow::NavigateToPrevDiff()
{
  size_t idx = m_pDocument->PrevDiffIndex();

  // Scroll y to prev diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_TextArea1.Y());
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
  m_TextArea1.ScrollLeftToColumn(newX);
  m_TextArea2.ScrollLeftToColumn(newX);
}


void DiffWindow::YChangedHandler(size_t newY)
{
  m_TextArea1.ScrollTopToRow(newY);
  m_TextArea2.ScrollTopToRow(newY);
}


void DiffWindow::XIncrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_TextArea1.scrollLeft(numChars);
  m_TextArea2.scrollLeft(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollLeft(m_TextArea1.X());
  }
}


void DiffWindow::XDecrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_TextArea1.scrollRight(numChars);
  m_TextArea2.scrollRight(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollLeft(m_TextArea1.X());
  }
}


void DiffWindow::YIncrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_TextArea1.scrollUp(numLines);
  m_TextArea2.scrollUp(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_TextArea1.Y());
  }
}


void DiffWindow::YDecrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_TextArea1.scrollDown(numLines);
  m_TextArea2.scrollDown(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_TextArea1.Y());
  }
}


void DiffWindow::createGadgets()
{
  bool bFirstCall = (m_pGadtoolsContext == NULL);

  struct Gadget* pFakeGad = NULL;
  pFakeGad = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  if(pFakeGad == NULL)
  {
    return;
  }

  if(bFirstCall)
  {
    // When this method is called for the first time, the created
    // GadTools context either is set as first gadget or, if parent
    // already contains gadgets, it is linked as successor of parents
    // last gadget.

    if(m_pLastParentGadget == NULL)
    {
      setFirstGadget(m_pGadtoolsContext);
    }
    else
    {
      m_pLastParentGadget->NextGadget = m_pGadtoolsContext;
    }
  }

  WORD m_FontHeight =  m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;

  struct NewGadget newGadget;
  newGadget.ng_TextAttr   = m_Screen.IntuiTextAttr();
  newGadget.ng_VisualInfo = m_Screen.GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = m_TextArea1.Left();
  newGadget.ng_TopEdge    = m_TextArea1.Top() - m_FontHeight - 4;
  newGadget.ng_Height     = m_FontHeight + 2;
  newGadget.ng_Flags      = PLACETEXT_RIGHT | NG_HIGHLABEL;
  newGadget.ng_GadgetText = NULL;

  if(m_TextArea1.Width() == 0)
  {
    newGadget.ng_Width    = 120; // Some random default
  }
  else
  {
    newGadget.ng_Width    = m_TextArea1.Width();
  }

  const char* pFileName = NULL;
  if(m_pDocument != NULL)
  {
    pFileName = m_pDocument->RightFileName();
  }
  else
  {
    pFileName = &m_EmptyChar;
  }


  m_pGadTxtLeftFile = CreateGadget(TEXT_KIND,
                                   pFakeGad,
                                   &newGadget,
                                   GTTX_Border, TRUE,
                                   GTTX_Text, pFileName,
                                   TAG_DONE);

  if(m_TextArea2.Left() > 0)
  {
    newGadget.ng_LeftEdge = m_TextArea2.Left();
  }
  else
  {
    newGadget.ng_LeftEdge += newGadget.ng_Width + 5;
  }

  pFileName = NULL;
  if(m_pDocument != NULL)
  {
    pFileName = m_pDocument->RightFileName();
  }
  else
  {
    pFileName = &m_EmptyChar;
  }


  m_pGadTxtRightFile = CreateGadget(TEXT_KIND,
                                    m_pGadTxtLeftFile,
                                    &newGadget,
                                    GTTX_Border, TRUE,
                                    GTTX_Text, pFileName,
                                    TAG_DONE);
}



void DiffWindow::handleGadgetEvent(struct Gadget* pGadget)
{
  if(pGadget == NULL)
  {
    return;
  }

  // TODO
}


void DiffWindow::handleVanillaKey(UWORD code)
{
  // TODO
}


void DiffWindow::calcSizes()
{
  // (Re-)calculate some values that may have be changed by re-sizing
  ScrollbarWindow::calcSizes();

  long textAreasWidth = m_InnerWindowRight - m_TextArea1.Left() - m_IndentX;
  textAreasWidth /= 2;

  // Pre-calc text areas heigt. Will later be limited to int multiples.
  long textAreasHeight = m_InnerWindowBottom - m_TextArea1.Top() - m_IndentY;

  m_TextArea2.SetLeftTop(m_TextArea1.Left() + textAreasWidth, 
                         m_TextArea1.Top());


  // Set the dimension of the text areas
  m_TextArea1.SetSizes(textAreasWidth, 
                       textAreasHeight);

  m_TextArea2.SetSizes(textAreasWidth, 
                       textAreasHeight);

  if(m_pDocument == NULL)
  {
    return;
  }

  // Set x-scroll-gadget's pot size in relation of new window size
  setXScrollPotSize(m_TextArea1.NumVisibleChars(), 
                    m_pDocument->MaxLineLength());

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_TextArea1.NumVisibleLines(), 
                    m_pDocument->NumLines());
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
           m_TextArea1.Top() - 3);

  // Create the gadgets anew (with the new positions and size)
  createGadgets();

  AddGList(m_pWindow, m_pGadtoolsContext, (UWORD)~0, -1, NULL);
  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);
  FreeGadgets(pOldContext);

  if(m_pDocument != NULL)
  {
    // Display the document file names in the gadgets
    GT_SetGadgetAttrs(m_pGadTxtLeftFile,
                      m_pWindow,
                      NULL,
                      GTTX_Text, m_pDocument->LeftFileName(),
                      TAG_DONE);
  }

  if(m_pDocument != NULL)
  {
    GT_SetGadgetAttrs(m_pGadTxtRightFile,
                      m_pWindow,
                      NULL,
                      GTTX_Text, m_pDocument->RightFileName(),
                      TAG_DONE);
  }
}


void DiffWindow::paintWindowDecoration()
{
  // Create borders for the two text areas
  DrawBevelBox(m_pRPorts->Window(),
               m_TextArea1.Left(),
               m_TextArea1.Top(),
               m_TextArea1.Width(),
               m_TextArea1.Height(),
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  DrawBevelBox(m_pRPorts->Window(),
               m_TextArea2.Left(),
               m_TextArea2.Top(),
               m_TextArea2.Width(),
               m_TextArea2.Height(),
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);
}


void DiffWindow::paintStatusBar()
{
  if(!IsOpen())
  {
    return;
  }

  int top = m_TextArea1.Top() + m_TextArea1.Height() + m_InnerWindowBottom;
  top /= 2;
  top -= m_Screen.IntuiDrawInfo()->dri_Font->tf_Baseline;
  top++;

  int left = m_TextArea1.Left() + 2;

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
