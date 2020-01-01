#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include "DiffWindow.h"

extern struct GfxBase* GfxBase;


DiffWindow::DiffWindow(AScreen& appScreen,
                       struct MsgPort*& pIdcmpMsgPort,
                       int& numOpenWindows)
  : ScrollbarWindow(appScreen, pIdcmpMsgPort, numOpenWindows),
    m_pLeftDocument(NULL),
    m_pRightDocument(NULL),
    m_pLastParentGadget(NULL),
    m_pGadtoolsContext(NULL),
    m_pGadTxtLeftFile(NULL),
    m_pGadTxtRightFile(NULL),
    m_pDiffStartIdxsList(NULL),
    m_bNoNavigationDone(true),
    m_NumDifferences(0),
    m_bShowLineNumbers(false),
    m_LineNumsWidth_chars(0),
    m_LineNumsWidth_pix(0),
    m_X(0),
    m_Y(0),
    m_MaxTextAreaChars(0),
    m_MaxTextAreaLines(0),
    m_MaxLineLength(0),
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
    m_ScrollAreasYMax(0),
    m_ChangedText(" Changed "),
    m_AddedText(" Added "),
    m_DeletedText(" Deleted "),
    m_StatusBarText("No diff performed.")
{
  //
  // Get the width and height of a char of the systems default
  // monospace font.
  //
  struct TextFont* defaultTextFont =
    ((struct GfxBase *)GfxBase)->DefaultFont;

  m_FontWidth_pix = defaultTextFont->tf_XSize;
  m_FontHeight_pix = defaultTextFont->tf_YSize;
  m_FontBaseline_pix = defaultTextFont->tf_Baseline;
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


void DiffWindow::SetLineNumbers(bool bEnabled)
{
  m_bShowLineNumbers = bEnabled;
}


void DiffWindow::Resized()
{
  if(!IsOpen())
  {
    return;
  }

  if(m_pWindow->Width == m_WinWidth &&
     m_pWindow->Height == m_WinHeight)
  {
    // nothing changed
    return;
  }

  m_WinWidth = m_pWindow->Width;
  m_WinHeight = m_pWindow->Height;

  // Calculate some values which have to calculated after window
  // opening and after resizing
  calcSizes();

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AScreen.Pens().Background());

  // Resize gadgets to fit into new window size
  resizeGadgets();

  if((m_pLeftDocument != NULL) && m_pRightDocument != NULL)
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


bool DiffWindow::Open(const APTR pMenuItemDisableAtOpen,
                      InitialPosition initialPos)
{
  //
  // Calculate some initial values. It's possible that they are needed
  // in the initialize() method which is called from WindowBase::Open()
  // shortly before opening, so the Open() call is done afterwards.
  //
  m_IndentY = 2 * m_AScreen.FontHeight();
  m_TextArea1Left = m_IndentX;
  m_TextAreasTop = m_IndentY;

  m_TextAttr.ta_Name = m_AScreen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_AScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_AScreen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_AScreen.IntuiDrawInfo()->dri_Font->tf_Flags;

  //
  // Open the window
  //
  if(!ScrollbarWindow::Open(pMenuItemDisableAtOpen, initialPos))
  {
    return false;
  }

  // Calculate some sizes which are only calculatable with window
  // already open
  calcSizes();

  // With the calculated sizes the gadgets must be re-sized/positioned
  resizeGadgets();

  // Paint the window decoration
  paintWindowDecoration();

  return true;
}


bool DiffWindow::SetContent(DiffDocument* pLeftDocument,
                            DiffDocument* pRightDocument,
                            LinkedList* pDiffStartIdxsList,
                            long diffTime,
                            int numAdded,
                            int numChanged,
                            int numDeleted)
{
  if((pLeftDocument == NULL) || (pRightDocument == NULL))
  {
    return false;
  }

  m_pLeftDocument = pLeftDocument;
  m_pRightDocument = pRightDocument;
  m_pDiffStartIdxsList = pDiffStartIdxsList;

  m_NumDifferences = numAdded + numChanged + numDeleted;

  m_StatusBarText = "Diff performed in ";
  m_StatusBarText += diffTime;
  m_StatusBarText += " ms. Total changes: ";
  m_StatusBarText += m_NumDifferences;
  m_StatusBarText += "   |   ";

  SimpleString emptyStr = "";
  m_AddedText = emptyStr + numAdded + " Added ";
  m_ChangedText = emptyStr + numChanged + " Changed ";
  m_DeletedText = emptyStr + numDeleted + " Deleted ";

  m_bNoNavigationDone = true;
  m_X = 0;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AScreen.Pens().Background());

  // Display the document file names in the gadgets
  GT_SetGadgetAttrs(m_pGadTxtLeftFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pLeftDocument->FileName(),
                    TAG_END);

  GT_SetGadgetAttrs(m_pGadTxtRightFile,
                    m_pWindow,
                    NULL,
                    GTTX_Text, m_pRightDocument->FileName(),
                    TAG_END);


  // Ensure that the gadgets are re-drawn
  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);

  // Calculate the length of the longest line
  size_t maxCharsLeft = pLeftDocument->MaxLineLength();
  size_t maxCharsRight = pRightDocument->MaxLineLength();
  m_MaxLineLength = 0;
  if(maxCharsLeft > maxCharsRight)
  {
    m_MaxLineLength = maxCharsLeft;
  }
  else
  {
    m_MaxLineLength = maxCharsRight;
  }

  if(m_bShowLineNumbers)
  {
    const DiffLine* pLine = m_pLeftDocument->GetIndexedLine(0);
    const char* pLineNum = pLine->LineNum();

    m_LineNumsWidth_chars = strlen(pLineNum);
    m_LineNumsWidth_pix = m_LineNumsWidth_chars * m_FontWidth_pix;

    m_LineNumberEmpty = SimpleString(' ', m_LineNumsWidth_chars);
  }

  // Get the number of lines (will/should be equal for left and right)
  m_NumLines = m_pLeftDocument->NumLines();

  calcSizes();

  // Paint the window decoration
  paintWindowDecoration();

  // Display the first [1; m_MaxTextAreaLines] lines of the files
  paintDocuments();

  // Paint the status bar
  paintStatusBar();

  setXScrollPotSize(m_MaxTextAreaChars, m_MaxLineLength);

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxTextAreaLines, m_NumLines);

  return true;
}


void DiffWindow::XChangedHandler(size_t newX)
{
  int delta = newX - m_X;
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
  if(m_pDiffStartIdxsList == NULL)
  {
    return;
  }

  // Calculating the yLimit (max y allowed for scrolling) enables to
  // determine if we're already on the last page
  long yLimit = m_NumLines - m_MaxTextAreaLines;
  bool bAlreadyOnLastPage = false;
  if((yLimit > 0) && (m_Y >= yLimit))
  {
    bAlreadyOnLastPage = true;
  }

  size_t* pItem = NULL;
  if(m_bNoNavigationDone == true)
  {
    m_bNoNavigationDone = false;
    pItem = (size_t*)m_pDiffStartIdxsList->GetFirst();
  }
  else if(bAlreadyOnLastPage == true)
  {
    // When already on last page don't navigate through all the diffs
    // that maybe there. Instead jump to the first diff.
    pItem = (size_t*)m_pDiffStartIdxsList->GetFirst();
  }
  else
  {
    pItem = (size_t*)m_pDiffStartIdxsList->GetNext();
    if(pItem == NULL)
    {
      // There is no next diff. Jump to the first one.
      pItem = (size_t*)m_pDiffStartIdxsList->GetFirst();
    }
  }

  if(pItem == NULL)
  {
    return;
  }

  // Scroll y to next diff
  YChangedHandler(*pItem);

  // Set scrollbar to new y position
  setYScrollTop(m_Y);
}


void DiffWindow::NavigateToPrevDiff()
{
  if(m_pDiffStartIdxsList == NULL)
  {
    return;
  }

  // Calculating the yLimit (max y pos for scrolling)
  long yLimit = m_NumLines - m_MaxTextAreaLines;

  size_t* pItem = NULL;

  if(m_bNoNavigationDone == true)
  {
    m_bNoNavigationDone = false;
    pItem = (size_t*)m_pDiffStartIdxsList->GetLast();
  }
  else
  {
    pItem = (size_t*)m_pDiffStartIdxsList->GetPrev();

    if(pItem != NULL)
    {
      while((pItem != NULL) && (*pItem > yLimit))
      {
        pItem = (size_t*)m_pDiffStartIdxsList->GetPrev();
      }

    }
    else
    {
      // There is no prev diff. Jump to the last one.
      pItem = (size_t*)m_pDiffStartIdxsList->GetLast();

      // We want to select the upmost diff on the last page. So we
      // rewind from last diff by calling GetPrev until the diff idx
      // is less than yLimit. This is the first diff that isn't on that
      //  page anymore, so we call GetNext().
      while((pItem != NULL) && (*pItem > yLimit))
      {
        pItem = (size_t*)m_pDiffStartIdxsList->GetPrev();
      }

      pItem = (size_t*)m_pDiffStartIdxsList->GetNext();
    }
  }


  if(pItem == NULL)
  {
    return;
  }

  // Scroll y to prev diff
  YChangedHandler(*pItem);

  // Set scrollbar to new y position
  setYScrollTop(m_Y);
}


void DiffWindow::initialize()
{
  // Call parent method to ensure that the scroll gadgets in the window
  // borders are initialized first.
  ScrollbarWindow::initialize();

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
  setFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DEPTHGADGET |     // Add a depth gadget
           WFLG_SIZEGADGET |      // Add a size gadget
           WFLG_GIMMEZEROZERO);   // Different layers for border and content

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Inform about menu selection
           IDCMP_CLOSEWINDOW |    // Inform about click on close gadget
           IDCMP_NEWSIZE);        // Inform about resizing

  m_bInitialized = true;
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

  WORD m_FontHeight = m_AScreen.FontHeight();

  struct NewGadget newGadget;
  newGadget.ng_TextAttr   = m_AScreen.IntuiTextAttr();
  newGadget.ng_VisualInfo = m_AScreen.GadtoolsVisualInfo();
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
  if(m_pLeftDocument != NULL)
  {
    pFileName = m_pLeftDocument->FileName();
  }

  m_pGadTxtLeftFile = CreateGadget(TEXT_KIND,
                                   pFakeGad,
                                   &newGadget,
                                   GTTX_Border, TRUE,
                                   GTTX_Text, pFileName,
                                   TAG_END);

  if(m_TextArea2Left > 0)
  {
    newGadget.ng_LeftEdge = m_TextArea2Left;
  }
  else
  {
    newGadget.ng_LeftEdge += newGadget.ng_Width + 5;
  }

  pFileName = NULL;
  if(m_pRightDocument != NULL)
  {
    pFileName = m_pRightDocument->FileName();
  }


  m_pGadTxtRightFile = CreateGadget(TEXT_KIND,
                                    m_pGadTxtLeftFile,
                                    &newGadget,
                                    GTTX_Border, TRUE,
                                    GTTX_Text, pFileName,
                                    TAG_END);
}


bool DiffWindow::HandleIdcmp(ULONG msgClass,
                             UWORD msgCode,
                             APTR pItemAddress)
{
  if(ScrollbarWindow::HandleIdcmp(msgClass, msgCode, pItemAddress) == true)
  {
    return true;
  }

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
      handleGadgetEvent(pGadget);
      return true;
      break;
    }

    case IDCMP_VANILLAKEY:
    {
      handleVanillaKey(msgCode);
      return true;
      break;
    }

    case IDCMP_NEWSIZE:
    {
      Resized();
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
  m_MaxTextAreaChars = (m_TextAreasWidth - 4 - m_LineNumsWidth_pix)
                     / m_FontWidth_pix;

  // Set x-scroll-gadget's pot size in relation of new window size
  setXScrollPotSize(m_MaxTextAreaChars, m_MaxLineLength);

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
  m_ScrollArea1XMax = m_TextArea1Left + textAreasTextWidth_pix - 3;

  m_ScrollArea2XMinHScroll = m_TextArea2Left + m_LineNumsWidth_pix + 3;
  m_ScrollArea2XMinVScroll = m_TextArea2Left + 3;
  m_ScrollArea2XMax = m_TextArea2Left + textAreasTextWidth_pix - 3;

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
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Background());
  RectFill(m_pWindow->RPort,
           0,
           0,
           m_InnerWindowRight,
           m_TextAreasTop - 3);

  // Create the gadgets anew (with the new positions and size)
  createGadgets();

  AddGList(m_pWindow, m_pGadtoolsContext, (UWORD)~0, -1, NULL);
  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);
  FreeGadgets(pOldContext);

  if(m_pLeftDocument != NULL)
  {
    // Display the document file names in the gadgets
    GT_SetGadgetAttrs(m_pGadTxtLeftFile,
                      m_pWindow,
                      NULL,
                      GTTX_Text, m_pLeftDocument->FileName(),
                      TAG_END);
  }

  if(m_pRightDocument != NULL)
  {
    GT_SetGadgetAttrs(m_pGadTxtRightFile,
                      m_pWindow,
                      NULL,
                      GTTX_Text, m_pRightDocument->FileName(),
                      TAG_END);
  }
}


void DiffWindow::paintDocuments(bool bFromStart)
{
  if(m_pLeftDocument == NULL || m_pRightDocument == NULL)
  {
    return;
  }

  if(bFromStart == true)
  {
    m_X = 0;
    m_Y = 0;
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Text());

  for(size_t i = m_Y; (i - m_Y) < m_MaxTextAreaLines; i++)
  {
    if(i >= m_NumLines)
    {
      break;
    }

    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

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
                           int numChars)
{
  size_t indent = 0;

  if(startIndex < 0)
  {
    startIndex = m_X;
  }

  if(numChars < 0)
  {
    // A negative numChars means that the text is to be inserted
    // right-adjusted. So here an indent for the text is calculated
    // and numChars is made positive to get used below.
    numChars = -numChars;
    indent = (m_MaxTextAreaChars - numChars) * m_FontWidth_pix;
  }

  if(!bHorizontallyScrolled && m_bShowLineNumbers)
  {
    //
    // Print the line numbers in left and right
    //

    // Set the background color
    SetBPen(m_pWindow->RPort, m_AScreen.Pens().Gray());

    // Move rastport cursor to start of left line numbers block
    ::Move(m_pWindow->RPort,
           m_TextArea1Left + 2 + indent,
           topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLeftLine->LineNum();
    if(pLineNum == NULL)
    {
      pLineNum = m_LineNumberEmpty.C_str();
    }

    // Print the text
    Text(m_pWindow->RPort, pLineNum, m_LineNumsWidth_chars);

    // Move rastport cursor to start of right line numbers block
    ::Move(m_pWindow->RPort,
           m_TextArea2Left + 2 + indent,
           topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    pLineNum = pRightLine->LineNum();
    if(pLineNum == NULL)
    {
      pLineNum = m_LineNumberEmpty.C_str();
    }

    // Print the text
    Text(m_pWindow->RPort, pLineNum, m_LineNumsWidth_chars);

  }

  // Move rastport cursor to start of left line
  ::Move(m_pWindow->RPort,
         m_TextArea1Left + 3 + indent + m_LineNumsWidth_pix,
         topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

  // Set the left line's background color
  SetBPen(m_pWindow->RPort, colorNameToPen(m_pLeftDocument->LineColor()));

  int numCharsToPrint = 0;
  if(numChars > 0)
  {
    numCharsToPrint = numChars;
  }
  else
  {
    // Determine how many characters would be print theoretically
    numCharsToPrint = pLeftLine->NumChars() - m_X;
  }

  if(numCharsToPrint > m_MaxTextAreaChars)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_MaxTextAreaChars;
  }

  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pLeftLine->NumChars()))
  {
    // Limit the number of printed chars to line length
    numCharsToPrint = pLeftLine->NumChars() - startIndex;
  }

  if(numCharsToPrint > 0)
  {
    // Left line is visible regarding current x scroll: print it
    Text(m_pWindow->RPort,
         pLeftLine->Text() + startIndex,
         numCharsToPrint);
  }

  // Move rastport cursor to start of right line
  ::Move(m_pWindow->RPort,
         m_TextArea2Left + 3  + indent + m_LineNumsWidth_pix,
         topEdge + m_TextAreasTop + m_FontBaseline_pix + 1);

  // Set the right line's background color
  SetBPen(m_pWindow->RPort, colorNameToPen(m_pRightDocument->LineColor()));

  numCharsToPrint = 0;
  if(numChars > 0)
  {
    numCharsToPrint = numChars;
  }
  else
  {
    // Determine how many characters would be print theoretically
    numCharsToPrint = pRightLine->NumChars() - m_X;
  }

  if(numCharsToPrint > m_MaxTextAreaChars)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_MaxTextAreaChars;
  }

  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pRightLine->NumChars()))
  {
    // Limit the number of printed chars to line length
    numCharsToPrint = pRightLine->NumChars() - startIndex;
  }

  if(numCharsToPrint > 0)
  {
    // Right line is visible regarding current x scroll: print it
    Text(m_pWindow->RPort,
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
               GT_VisualInfo, m_AScreen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  DrawBevelBox(m_pWindow->RPort,
               m_TextArea2Left,
               m_TextAreasTop,
               m_TextAreasWidth,
               m_TextAreasHeight,
               GT_VisualInfo, m_AScreen.GadtoolsVisualInfo(),
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
  top -= m_AScreen.IntuiDrawInfo()->dri_Font->tf_Baseline;
  top++;

  int left = m_TextArea1Left + 2;

  // Clear the status bar area
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Background());
  RectFill(m_pWindow->RPort,
           0,
           top,
           m_InnerWindowRight,
           m_InnerWindowBottom);


  struct IntuiText intuiText;
  intuiText.FrontPen  = m_AScreen.Pens().Text();
  intuiText.BackPen   = m_AScreen.Pens().Background();
  intuiText.DrawMode  = JAM2;
  intuiText.ITextFont = &m_TextAttr;
  intuiText.NextText  = NULL;

  intuiText.TopEdge   = top;
  intuiText.LeftEdge  = left;
  intuiText.IText = (UBYTE*) m_StatusBarText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText);
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AScreen.Pens().Green();
  intuiText.IText = (UBYTE*) m_AddedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AScreen.Pens().Yellow();
  intuiText.IText = (UBYTE*) m_ChangedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AScreen.Pens().Red();
  intuiText.IText = (UBYTE*) m_DeletedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);
}


LONG DiffWindow::colorNameToPen(DiffDocument::ColorName colorName)
{
  if(colorName == DiffDocument::CN_Green)
  {
    return m_AScreen.Pens().Green();
  }
  else if(colorName== DiffDocument::CN_Yellow)
  {
    return m_AScreen.Pens().Yellow();
  }
  else if(colorName == DiffDocument::CN_Red)
  {
    return m_AScreen.Pens().Red();
  }
  else
  {
    return m_AScreen.Pens().Background();
  }
}


size_t DiffWindow::scrollRight(int numChars)
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


  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AScreen.Pens().Background());

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
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

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


size_t DiffWindow::scrollLeft(int numChars)
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

  if(m_MaxLineLength < m_MaxTextAreaChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxTextAreaChars) == m_MaxLineLength)
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_MaxTextAreaChars + numChars) > m_MaxLineLength)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_MaxLineLength - (m_X + m_MaxTextAreaChars);
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AScreen.Pens().Background());

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
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

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


size_t DiffWindow::scrollDown(int numLines)
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

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AScreen.Pens().Background());

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
  for(int i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(lineIndex);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(lineIndex);

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


size_t DiffWindow::scrollUp(int numLines)
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

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AScreen.Pens().Background());

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

  for(int i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y + m_MaxTextAreaLines + i;
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(lineIndex);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(lineIndex);

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

