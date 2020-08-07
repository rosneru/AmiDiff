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
                       struct MsgPort* pIdcmpMsgPort,
                       AMenu* pMenu)
  : ScrollbarWindow(screen, pIdcmpMsgPort, pMenu),
    m_pDocument(NULL),
    m_EmptyChar('\0'),
    m_pLastParentGadget(NULL),
    m_pGadtoolsContext(NULL),
    m_pGadTxtLeftFile(NULL),
    m_pGadTxtRightFile(NULL),
    m_bNoNavigationDone(true),
    m_NumDifferences(0),
    m_LineNumsWidth_chars(0),
    m_LineNumsWidth_pix(0),
    m_FontWidth_pix(m_pTextFont->tf_XSize),
    m_FontHeight_pix(m_pTextFont->tf_YSize),
    m_FontBaseline_pix(m_pTextFont->tf_Baseline),
    m_X(0),
    m_Y(0),
    m_MaxTextAreaChars(0),
    m_MaxTextAreaLines(0),
    m_NumLines(0),
    m_IndentX(5),
    m_IndentY(0),
    m_TextArea1Left(0),
    m_TextArea2Left(0),
    m_TextAreasTop(0),
    m_TextAreasWidth(0),
    m_TextAreasHeight(0),
    m_ScrollArea1XMinHScroll(0),
    m_ScrollArea1XMinVScroll(0),
    m_ScrollArea1XMax(0),
    m_ScrollArea2XMinHScroll(0),
    m_ScrollArea2XMinVScroll(0),
    m_ScrollArea2XMax(0),
    m_ScrollAreasYMin(0),
    m_ScrollAreasYMax(0)
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
  SetRast(m_pWindow->RPort, m_Screen.Pens().Background());

  // Resize gadgets to fit into new window size
  resizeGadgets();

  if(m_pDocument != NULL)
  {
    // Paint the document names
    //paintDocumentNames();
    // Now done in resizeGadgets()->createGadgets()

    // Paint the content of the two documents
    paintDocuments(false);
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

  m_pDocument = NULL;

  //
  // Calculate some initial values. It's possible that they are needed
  // in the initialize() method which is called from WindowBase::Open()
  // shortly before opening, so the Open() call is done afterwards.
  //
  m_IndentY = 2 *  m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextArea1Left = m_IndentX;
  m_TextAreasTop = m_IndentY;

  m_TextAttr.ta_Name = m_Screen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_Screen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_Screen.IntuiDrawInfo()->dri_Font->tf_Flags;

  SetAPen(m_pWindow->RPort, m_Screen.Pens().Text());

  m_RPortAPenBackgr = *m_pWindow->RPort;
  SetAPen(&m_RPortAPenBackgr, m_Screen.Pens().Background());

  m_RPortLineNum = *m_pWindow->RPort;
  SetBPen(&m_RPortLineNum, m_Screen.Pens().Gray());

  m_RPortTextDefault = *m_pWindow->RPort;
  SetBPen(&m_RPortTextDefault, m_Screen.Pens().Background());

  m_RPortTextRedBG = *m_pWindow->RPort;
  SetBPen(&m_RPortTextRedBG, m_Screen.Pens().Red());

  m_RPortTextGreenBG = *m_pWindow->RPort;
  SetBPen(&m_RPortTextGreenBG, m_Screen.Pens().Green());

  m_RPortTextYellowBG = *m_pWindow->RPort;
  SetBPen(&m_RPortTextYellowBG, m_Screen.Pens().Yellow());

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

  m_bNoNavigationDone = true;
  m_X = 0;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_Screen.Pens().Background());

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

  if(pDiffDocument->LineNumbersEnabled())
  {
    const DiffLine* pLine = m_pDocument->LeftLine(0);
    const char* pLineNum = pLine->LineNum();

    m_LineNumsWidth_chars = strlen(pLineNum);
    m_LineNumsWidth_pix = m_LineNumsWidth_chars * m_FontWidth_pix;
  }

  // Get the number of lines (will/should be equal for left and right)
  m_NumLines = m_pDocument->NumLines();

  calcSizes();

  // Paint the window decoration
  paintWindowDecoration();

  // Display the first [1; m_MaxTextAreaLines] lines of the files
  paintDocuments();

  // Paint the status bar
  paintStatusBar();

  setXScrollPotSize(m_MaxTextAreaChars, m_pDocument->MaxLineLength());

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxTextAreaLines, m_NumLines);

  return true;
}


void DiffWindow::XChangedHandler(size_t newX)
{
  long delta = newX - m_X;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);

  //int deltaLimit = m_MaxTextAreaChars / 2;
  int deltaLimit = 10;

  //
  // Scroll small amounts (1/2 text area width) line by line
  //
  if(deltaAbs < deltaLimit)
  {
    if(delta > 0)
    {
      m_X += scrollLeft(deltaAbs);
    }
    else if(delta < 0)
    {
      m_X -= scrollRight(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // x-position
  //
  m_X = newX;

  // Clear both text areas completely
  EraseRect(m_pWindow->RPort,
    m_TextArea1Left + 2, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  EraseRect(m_pWindow->RPort,
    m_TextArea2Left + 2, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  paintDocuments(false);

}


void DiffWindow::YChangedHandler(size_t newY)
{
  // Prevent to scroll below the last line
  long yLimit = m_NumLines - m_MaxTextAreaLines;
  if((yLimit > 0) && (newY > yLimit))
  {
    newY = yLimit;
  }

  int delta = newY - m_Y;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);

  int deltaLimit = m_MaxTextAreaLines - 2;
  //int deltaLimit = 10;

  //
  // Scroll small amounts (1/2 text area height) line by line
  //
  if(deltaAbs < deltaLimit)
  {
    if(delta > 0)
    {
      m_Y += scrollUp(deltaAbs);
    }
    else if(delta < 0)
    {
      m_Y -= scrollDown(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // y-position
  //
  m_Y = newY;

  // Clear both text areas completely
  EraseRect(m_pWindow->RPort,
    m_TextArea1Left + 2, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  EraseRect(m_pWindow->RPort,
    m_TextArea2Left + 2, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  paintDocuments(false);
}


void DiffWindow::XIncrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_X += scrollLeft(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollLeft(m_X);
  }
}


void DiffWindow::XDecrease(size_t numChars,
                           bool bTriggeredByScrollPot)
{
  m_X -= scrollRight(numChars);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollLeft(m_X);
  }
}


void DiffWindow::YIncrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_Y += scrollUp(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_Y);
  }
}


void DiffWindow::YDecrease(size_t numLines,
                           bool bTriggeredByScrollPot)
{
  m_Y -= scrollDown(numLines);

  if(!bTriggeredByScrollPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_Y);
  }
}


void DiffWindow::NavigateToNextDiff()
{
  // Calculating the yLimit (max y allowed for scrolling) enables to
  // determine if we're already on the last page
  size_t yLimit = m_NumLines - m_MaxTextAreaLines;
  bool bAlreadyOnLastPage = false;
  if((yLimit > 0) && (m_Y >= yLimit))
  {
    bAlreadyOnLastPage = true;
  }

  size_t idx = 0;
  if(m_bNoNavigationDone == true)
  {
    m_bNoNavigationDone = false;
    idx = m_pDocument->FirstDiffIndex();
  }
  else if(bAlreadyOnLastPage == true)
  {
    // When already on last page don't navigate through all the diffs
    // that maybe there. Instead jump to the first diff.
    idx = m_pDocument->FirstDiffIndex();
  }
  else
  {
    idx = m_pDocument->NextDiffIndex();
  }

  // Scroll y to next diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_Y);
}


void DiffWindow::NavigateToPrevDiff()
{
  // Calculating the yLimit (max y pos for scrolling)
  size_t yLimit = m_NumLines - m_MaxTextAreaLines;

  size_t idx = NULL;

  if(m_bNoNavigationDone == true)
  {
    m_bNoNavigationDone = false;
    idx = m_pDocument->LastDiffIndex();
  }
  else
  {
    idx = m_pDocument->PrevDiffIndex();

    if(idx != NULL)
    {
      while(idx > yLimit)
      {
        idx = m_pDocument->PrevDiffIndex();
      }

    }
    else
    {
      // There is no prev diff. Jump to the last one.
      idx = m_pDocument->LastDiffIndex();

      // We want to select the upmost diff on the last page. So we
      // rewind from last diff by calling GetPrev until the diff idx
      // is less than yLimit. This is the first diff that isn't on that
      //  page anymore, so we call GetNext().
      while(idx > yLimit)
      {
        idx = m_pDocument->PrevDiffIndex();
      }

      idx = m_pDocument->NextDiffIndex();
    }
  }

  // Scroll y to prev diff
  YChangedHandler(idx);

  // Set scrollbar to new y position
  setYScrollTop(m_Y);
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
  newGadget.ng_LeftEdge   = m_TextArea1Left;
  newGadget.ng_TopEdge    = m_TextAreasTop - m_FontHeight - 4;
  newGadget.ng_Height     = m_FontHeight + 2;
  newGadget.ng_Flags      = PLACETEXT_RIGHT | NG_HIGHLABEL;
  newGadget.ng_GadgetText = NULL;

  if(m_TextAreasWidth == 0)
  {
    newGadget.ng_Width    = 120; // Some random default
  }
  else
  {
    newGadget.ng_Width    = m_TextAreasWidth;
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

  if(m_TextArea2Left > 0)
  {
    newGadget.ng_LeftEdge = m_TextArea2Left;
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

  m_TextAreasWidth = m_InnerWindowRight - m_TextArea1Left - m_IndentX;
  m_TextAreasWidth /= 2;

  // Pre-calc text areas heigt. Will later be limited to int multiples.
  m_TextAreasHeight = m_InnerWindowBottom - m_TextAreasTop - m_IndentY;

  m_TextArea2Left = m_TextArea1Left + m_TextAreasWidth;

  if((m_FontHeight_pix == 0) || (m_FontWidth_pix == 0))
  {
    return;
  }

  // Calculate how many lines fit into each text area
  m_MaxTextAreaLines = (m_TextAreasHeight - 4) /  m_FontHeight_pix;

  // Limit text areas heigt to int multiples.
  m_TextAreasHeight = m_MaxTextAreaLines * m_FontHeight_pix + 3;

  // Calculate how many chars fit on each line in each text area
  m_MaxTextAreaChars = (m_TextAreasWidth - 7 - m_LineNumsWidth_pix)
                     / m_FontWidth_pix;

  if(m_pDocument != NULL)
  {
    // Set x-scroll-gadget's pot size in relation of new window size
    setXScrollPotSize(m_MaxTextAreaChars, m_pDocument->MaxLineLength());
  }

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_MaxTextAreaLines, m_NumLines);

  //
  // Define the dimensions for the scroll areas
  // NOTE: The XMin values depend on if the scroll is done horizontally
  //       or vertically. On vertical scroll the line numbers are
  //       scrolled, too. On horizontal scroll, they're not.
  //
  LONG textAreasTextWidth_pix = m_MaxTextAreaChars * m_FontWidth_pix;
  textAreasTextWidth_pix += m_LineNumsWidth_pix;

  m_ScrollArea1XMinHScroll = m_TextArea1Left + m_LineNumsWidth_pix + 3;
  m_ScrollArea1XMinVScroll = m_TextArea1Left + 3;
  m_ScrollArea1XMax = m_TextArea1Left + textAreasTextWidth_pix + 2;

  m_ScrollArea2XMinHScroll = m_TextArea2Left + m_LineNumsWidth_pix + 3;
  m_ScrollArea2XMinVScroll = m_TextArea2Left + 3;
  m_ScrollArea2XMax = m_TextArea2Left + textAreasTextWidth_pix + 2;

  m_ScrollAreasYMin = m_TextAreasTop + 1;
  m_ScrollAreasYMax = m_TextAreasTop + m_TextAreasHeight - 3;

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
  RectFill(&m_RPortAPenBackgr,
           0,
           0,
           m_InnerWindowRight,
           m_TextAreasTop - 3);

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


void DiffWindow::paintDocuments(bool bFromStart)
{
  if(m_pDocument == NULL)
  {
    return;
  }

  if(bFromStart == true)
  {
    m_X = 0;
    m_Y = 0;
  }

  for(size_t i = m_Y; (i - m_Y) < m_MaxTextAreaLines; i++)
  {
    if(i >= m_NumLines)
    {
      break;
    }

    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, pRightLine, (i - m_Y) * m_FontHeight_pix);
  }
}


void DiffWindow::paintLine(const DiffLine* pLeftLine,
                           const DiffLine* pRightLine,
                           WORD topEdge,
                           bool bHorizontallyScrolled,
                           int startIndex,
                           int count)
{
  size_t indent = 0;

  if(startIndex < 0)
  {
    startIndex = m_X;
  }

  if(count < 0)
  {
    // A negative count means that the text is to be inserted
    // right-adjusted. So here an indent for the text is calculated
    // and numChars is made positive to get used below.
    count = -count;
    indent = (m_MaxTextAreaChars - count) * m_FontWidth_pix;
  }

  if(!bHorizontallyScrolled && m_pDocument->LineNumbersEnabled())
  {
    //
    // Print the line numbers in left and right
    //

    // Move rastport cursor to start of left line numbers block
    ::Move(&m_RPortLineNum,
           m_TextArea1Left + indent + 2,
           topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLeftLine->LineNum();

    // Print left line's original line number
    Text(&m_RPortLineNum, pLineNum, m_LineNumsWidth_chars);

    // Move rastport cursor to start of right line numbers block
    ::Move(&m_RPortLineNum,
           m_TextArea2Left + indent + 2,
           topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    pLineNum = pRightLine->LineNum();

    // Print right line's original line number
    Text(&m_RPortLineNum, pLineNum, m_LineNumsWidth_chars);
  }

  // Getting the RastPort for the left line to draw in. This depends on
  // the line background color which itself depends on the diff state
  // of the line.
  RastPort* pRPort = diffStateToRastPort(pLeftLine->State());

  long numCharsToPrint = calcNumPrintChars(pLeftLine, count, startIndex);
  if(numCharsToPrint > 0)
  {
    // Move rastport cursor to start of left line
    ::Move(pRPort,
          m_TextArea1Left + indent + m_LineNumsWidth_pix + 3,
          topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Print left line
    Text(pRPort,
         pLeftLine->Text() + startIndex,
         numCharsToPrint);
  }

  // Getting the RastPort for the right line to draw in. This depends
  // on the line background color which itself depends on the diff
  // state of the line.
  pRPort = diffStateToRastPort(pRightLine->State());

  numCharsToPrint = calcNumPrintChars(pRightLine, count, startIndex);
  if(numCharsToPrint > 0)
  {
    // Move rastport cursor to start of right line
    ::Move(pRPort,
          m_TextArea2Left  + indent + m_LineNumsWidth_pix + 3,
          topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Print the right line
    Text(pRPort,
         pRightLine->Text() + startIndex,
         numCharsToPrint);
  }
}


void DiffWindow::paintWindowDecoration()
{
  // Create borders for the two text areas
  DrawBevelBox(m_pWindow->RPort,
               m_TextArea1Left,
               m_TextAreasTop,
               m_TextAreasWidth,
               m_TextAreasHeight,
               GT_VisualInfo, m_Screen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  DrawBevelBox(m_pWindow->RPort,
               m_TextArea2Left,
               m_TextAreasTop,
               m_TextAreasWidth,
               m_TextAreasHeight,
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

  int top = m_TextAreasTop + m_TextAreasHeight + m_InnerWindowBottom;
  top /= 2;
  top -= m_Screen.IntuiDrawInfo()->dri_Font->tf_Baseline;
  top++;

  int left = m_TextArea1Left + 2;

  // Clear the status bar area
  RectFill(&m_RPortAPenBackgr,
           0,
           top,
           m_InnerWindowRight,
           m_InnerWindowBottom);


  struct IntuiText intuiText;
  intuiText.FrontPen  = m_Screen.Pens().Text();
  intuiText.BackPen   = m_Screen.Pens().Background();
  intuiText.DrawMode  = JAM2;
  intuiText.ITextFont = &m_TextAttr;
  intuiText.NextText  = NULL;

  intuiText.TopEdge   = top;
  intuiText.LeftEdge  = left;
  intuiText.IText = (UBYTE*) m_StatusBarText;
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText);
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Screen.Pens().Green();
  intuiText.IText = (UBYTE*) m_AddedText;
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Screen.Pens().Yellow();
  intuiText.IText = (UBYTE*) m_ChangedText;
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_Screen.Pens().Red();
  intuiText.IText = (UBYTE*) m_DeletedText;
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);
}


LONG DiffWindow::calcNumPrintChars(const DiffLine* pDiffLine,
                                     int count,
                                     int startIndex)
{
  LONG numCharsToPrint = 0;

  if(count > 0)
  {
    numCharsToPrint = count;
  }
  else
  {
    // Determine how many characters would be print theoretically
    numCharsToPrint = pDiffLine->NumChars() - m_X;
  }

  if(numCharsToPrint > m_MaxTextAreaChars)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_MaxTextAreaChars;
  }

  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pDiffLine->NumChars()))
  {
    // Limit the number of printed chars to line length
    numCharsToPrint = pDiffLine->NumChars() - startIndex;
  }

  return numCharsToPrint;
}


RastPort* DiffWindow::diffStateToRastPort(DiffLine::LineState state)
{
  if(state == DiffLine::Added)
  {
    return &m_RPortTextGreenBG;
  }
  else if(state== DiffLine::Changed)
  {
    return &m_RPortTextYellowBG;
  }
  else if(state == DiffLine::Deleted)
  {
    return &m_RPortTextRedBG;
  }
  else
  {
    return &m_RPortTextDefault;
  }
}


size_t DiffWindow::scrollRight(size_t numChars)
{
  if(numChars < 1)
  {
    // Nothing to do
    return 0;
  }

  if(m_X < 1)
  {
    // Do not move the text area right if text is already at leftmost position
    return 0;
  }

  if(numChars > m_X)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_X;
  }

  if(numChars > m_MaxTextAreaChars)
  {
    numChars = m_MaxTextAreaChars;
  }


  // Set background color before scrolling TODO remove
  SetBPen(m_pWindow->RPort, m_Screen.Pens().Background());

  // Move each text area right by n * the height of one text line
  ScrollRasterBF(m_pWindow->RPort,
                 -numChars * m_FontWidth_pix, // n * width
                 0,
                 m_ScrollArea1XMinHScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea1XMax,
                 m_ScrollAreasYMax);

  ScrollRasterBF(m_pWindow->RPort,
                 -numChars * m_FontWidth_pix,  // n * width
                 0,
                 m_ScrollArea2XMinHScroll,
                 m_TextAreasTop + 1,
                 m_ScrollArea2XMax,
                 m_ScrollAreasYMax);

  // fill the gap with the previous chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine,
              pRightLine,
              (i - m_Y) * m_FontHeight_pix,
              true,
              m_X - numChars,
              numChars);

  }

  return numChars;
}


size_t DiffWindow::scrollLeft(size_t numChars)
{
  if(numChars < 1)
  {
    // Noting to do
    return 0;
  }

  if(numChars > m_MaxTextAreaChars)
  {
    numChars = m_MaxTextAreaChars;
  }

  if(m_pDocument->MaxLineLength() < m_MaxTextAreaChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxTextAreaChars) == m_pDocument->MaxLineLength())
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_MaxTextAreaChars + numChars) > m_pDocument->MaxLineLength())
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_pDocument->MaxLineLength() - (m_X + m_MaxTextAreaChars);
  }

  // Set background color before scrolling TODO remove
  SetBPen(m_pWindow->RPort, m_Screen.Pens().Background());

  // Move each text area left by n * the width of one char
  ScrollRasterBF(m_pWindow->RPort,
                 numChars * m_FontWidth_pix,
                 0,
                 m_ScrollArea1XMinHScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea1XMax,
                 m_ScrollAreasYMax);

  ScrollRasterBF(m_pWindow->RPort,
                 numChars * m_FontWidth_pix,
                 0,
                 m_ScrollArea2XMinHScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea2XMax,
                 m_ScrollAreasYMax);

  // Fill the gap with the following chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine,
              pRightLine,
              (i - m_Y)  * m_FontHeight_pix,
              true,
              m_X + m_MaxTextAreaChars,
              -numChars);

  }

  return numChars;
}


size_t DiffWindow::scrollDown(size_t numLines)
{
  if(numLines < 1)
  {
    // Nothing to do
    return 0;
  }

  if(m_Y < 1)
  {
    // Do not move the text area downward if text is already at top
    return 0;
  }

  if(numLines > m_Y)
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    numLines = m_Y;
  }

  // Set background color before scrolling TODO remove
  SetBPen(m_pWindow->RPort, m_Screen.Pens().Background());

  // Move each text area downward by n * the height of one text line
  ScrollRasterBF(m_pWindow->RPort,
                 0,
                 -numLines * m_FontHeight_pix,  // n * height
                 m_ScrollArea1XMinVScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea1XMax,
                 m_ScrollAreasYMax);

  ScrollRasterBF(m_pWindow->RPort,
                 0,
                 -numLines * m_FontHeight_pix,  // n * height
                 m_ScrollArea2XMinVScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea2XMax,
                 m_ScrollAreasYMax);

  // Fill the gap with the previous text lines
  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pDocument->LeftLine(lineIndex);
    const DiffLine* pRightLine = m_pDocument->RightLine(lineIndex);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine,
              pRightLine,
              i * m_FontHeight_pix);
  }

  return numLines;
}


size_t DiffWindow::scrollUp(size_t numLines)
{
  if(numLines < 1)
  {
    // Noting to do
    return 0;
  }

  if(m_NumLines < m_MaxTextAreaLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_MaxTextAreaLines) == m_NumLines)
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_MaxTextAreaLines + numLines) > m_NumLines)
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    numLines = m_NumLines - (m_Y + m_MaxTextAreaLines);
  }


  // Set background color before scrolling TODO remove
  SetBPen(m_pWindow->RPort, m_Screen.Pens().Background());

  // Move each text area upward by n * the height of one text line
  ScrollRasterBF(m_pWindow->RPort,
                 0,
                 numLines * m_FontHeight_pix,
                 m_ScrollArea1XMinVScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea1XMax,
                 m_ScrollAreasYMax);

  ScrollRasterBF(m_pWindow->RPort,
                 0,
                 numLines * m_FontHeight_pix,
                 m_ScrollArea2XMinVScroll,
                 m_ScrollAreasYMin,
                 m_ScrollArea2XMax,
                 m_ScrollAreasYMax);

  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y + m_MaxTextAreaLines + i;
    const DiffLine* pLeftLine = m_pDocument->LeftLine(lineIndex);
    const DiffLine* pRightLine = m_pDocument->RightLine(lineIndex);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    int paintLineIndex = m_MaxTextAreaLines - numLines + i;
    paintLine(pLeftLine,
              pRightLine,
              paintLineIndex * m_FontHeight_pix);
  }

  return numLines;
}

