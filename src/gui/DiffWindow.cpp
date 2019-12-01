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

DiffWindow::DiffWindow(AppScreen& appScreen,
                       struct MsgPort* pMsgPort,
                       int& numOpenWindows)
  : ScrollbarWindow(appScreen, pMsgPort, numOpenWindows),
    m_pLeftDocument(NULL),
    m_pRightDocument(NULL),
    m_pLastParentGadget(NULL),
    m_pGadtoolsContext(NULL),
    m_pGadTxtLeftFile(NULL),
    m_pGadTxtRightFile(NULL),
    m_TextFontWidth_pix(0),
    m_TextFontHeight_pix(0),
    m_X(0),
    m_Y(0),
    m_MaxTextAreaChars(0),
    m_MaxTextAreaLines(0),
    m_MaxLineLength(0),
    m_IndentX(5),
    m_IndentY(0),
    m_TextArea1Left(0),
    m_TextArea2Left(0),
    m_TextAreasTop(0),
    m_TextAreasWidth(0),
    m_TextAreasHeight(0),
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

  m_TextFontWidth_pix = defaultTextFont->tf_XSize;
  m_TextFontHeight_pix = defaultTextFont->tf_YSize;
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
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Resize gadgets to fit into new window size
  resizeGadgets();

  if((m_pLeftDocument != NULL) && m_pRightDocument != NULL)
  {
    // Paint the document names
    //paintDocumentNames();
    // Now done in resizeGadgets()->createGadgets()

    // Paint the content of the two documents
    paintDocument();
  }

  // Paint the status bar
  paintStatusBar();

  // Paint the window decoration
  paintWindowDecoration();

}

bool DiffWindow::Open(const APTR pMenuItemDisableAtOpen,
                      InitialPosition initialPosition)
{
  //
  // Calculate some initial values. It's possible that they are needed
  // in the initialize() method which is called from WindowBase::Open()
  // shortly before opening, so the Open() call is done afterwards.
  //
  m_IndentY = 2 * m_AppScreen.FontHeight();
  m_TextArea1Left = m_IndentX;
  m_TextAreasTop = m_IndentY;

  m_TextAttr.ta_Name = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Flags;

  //
  // Open the window
  //
  if(!ScrollbarWindow::Open(pMenuItemDisableAtOpen, initialPosition))
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
                            DiffDocument* pRightDocument)
{
  if((pLeftDocument == NULL) || (pRightDocument == NULL))
  {
    return false;
  }

  m_pLeftDocument = pLeftDocument;
  m_pRightDocument = pRightDocument;
  m_X = 0;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

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

  // Display the first [1; m_MaxTextAreaLines] lines
  paintDocument();

  // Paint the window decoration
  paintWindowDecoration();

  // Paint the status bar
  paintStatusBar();

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

  setXScrollPotSize(m_MaxTextAreaChars, m_MaxLineLength);

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxTextAreaLines, m_pLeftDocument->NumLines());

  return true;
}

void DiffWindow::SetStatusBar(long diffTime, int numAdded,
                              int p_NumChanged, int numDeleted)
{
  long totalChanges = numAdded + p_NumChanged + numDeleted;

  m_StatusBarText = "Diff performed in ";
  m_StatusBarText += diffTime;
  m_StatusBarText += " ms. Total changes: ";
  m_StatusBarText += totalChanges;


  SimpleString emptyStr = "";
  m_AddedText = emptyStr + numAdded + " Added ";
  m_ChangedText = emptyStr + p_NumChanged + " Changed ";
  m_DeletedText = emptyStr + numDeleted + " Deleted ";

  paintStatusBar();
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

  paintDocument(false);

}

void DiffWindow::YChangedHandler(size_t newY)
{
  int delta = newY - m_Y;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);

//  int deltaLimit = m_MaxTextAreaLines / 2;
  int deltaLimit = 10;

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

  paintDocument(false);
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
  setIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_CLOSEWINDOW |    // Inform us about click on close gadget
           IDCMP_NEWSIZE);        // Inform us about resizing

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

  WORD m_FontHeight = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;

  struct NewGadget newGadget;
  newGadget.ng_TextAttr   = m_AppScreen.GfxTextAttr();
  newGadget.ng_VisualInfo = m_AppScreen.GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = m_TextArea1Left;
  newGadget.ng_TopEdge    = m_TextAreasTop - m_AppScreen.FontHeight() - 4;
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

  m_TextAreasHeight = m_InnerWindowBottom - m_TextAreasTop - m_IndentY;

  m_TextArea2Left = m_TextArea1Left + m_TextAreasWidth;

  if((m_TextFontHeight_pix == 0) || (m_TextFontWidth_pix == 0))
  {
    return;
  }

  // Calculate how many lines fit into each text area
  m_MaxTextAreaLines = (m_TextAreasHeight - 4) /  m_TextFontHeight_pix;

  // Calculate how many chars fit on each line in each text area
  m_MaxTextAreaChars = (m_TextAreasWidth - 4) / m_TextFontWidth_pix;

  // Set x-scroll-gadget's pot size in relation of new window size
  setXScrollPotSize(m_MaxTextAreaChars, m_MaxLineLength);

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_MaxTextAreaLines, m_pLeftDocument->NumLines());
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
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Background());
  RectFill(m_pWindow->RPort,
           m_TextArea1Left,
           m_TextAreasTop - m_AppScreen.FontHeight() - 4,
           m_TextAreasWidth + m_TextAreasWidth + 4,
           m_TextAreasTop - 3);

  // Create the gadgets anew (with the new positions and size)
  createGadgets();

  AddGList(m_pWindow, m_pGadtoolsContext, (UWORD)~0, -1, NULL);

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

  RefreshGList(m_pGadtoolsContext, m_pWindow, NULL, -1);

  FreeGadgets(pOldContext);
}


void DiffWindow::paintDocument(bool  fromStart)
{
  if(m_pLeftDocument == NULL || m_pRightDocument == NULL)
  {
    return;
  }

  if(fromStart == true)
  {
    m_X = 0;
    m_Y = 0;
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  for(size_t i = m_Y; (i - m_Y) < m_MaxTextAreaLines; i++)
  {
    if(i >= m_pLeftDocument->NumLines())
    {
      break;
    }

    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, pRightLine, (i - m_Y) * m_TextFontHeight_pix);
  }
}

void DiffWindow::paintLine(const DiffLine* pLeftLine,
                           const DiffLine* pRightLine,
                           WORD topEdge,
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
    indent = (m_MaxTextAreaChars - numChars) * m_TextFontWidth_pix;
  }

  // Move rastport cursor to start of left line
  ::Move(m_pWindow->RPort,
    m_TextArea1Left + 3 + indent,
    topEdge + m_TextAreasTop + m_TextFontHeight_pix);

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
    numCharsToPrint = pLeftLine->Length() - m_X;
  }

  // Limit the number of printed chars to fit into the text area
  if(numCharsToPrint > m_MaxTextAreaChars)
  {
    numCharsToPrint = m_MaxTextAreaChars;
  }

  // When p_StartChar is set: limit the number of printed chars to not
  // exceed the line length
  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pLeftLine->Length()))
  {
    numCharsToPrint = pLeftLine->Length() - startIndex;
  }

  // Print the left line if is visible regarding current x scroll
  if(numCharsToPrint > 0)
  {
    Text(m_pWindow->RPort,
      pLeftLine->Text() + startIndex,
      numCharsToPrint
    );
  }

  // Move rastport cursor to start of right line
  ::Move(m_pWindow->RPort,
    m_TextArea2Left + 3  + indent,
    topEdge + m_TextAreasTop + m_TextFontHeight_pix
  );

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
    numCharsToPrint = pRightLine->Length() - m_X;
  }

  // Limit the number of printed chars to fit into the text area
  if(numCharsToPrint > m_MaxTextAreaChars)
  {
    numCharsToPrint = m_MaxTextAreaChars;
  }

  // When p_StartChar is set: limit the number of printed chars to not
  // exceed the line length
  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pRightLine->Length()))
  {
    numCharsToPrint = pRightLine->Length() - startIndex;
  }

  // Print the right line if is visible regarding current x scroll
  if(numCharsToPrint > 0)
  {
    Text(m_pWindow->RPort,
      pRightLine->Text() + startIndex,
      numCharsToPrint
    );
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
               GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);

  DrawBevelBox(m_pWindow->RPort,
               m_TextArea2Left,
               m_TextAreasTop,
               m_TextAreasWidth,
               m_TextAreasHeight,
               GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
               GTBB_Recessed, TRUE,
               TAG_DONE);
}


void DiffWindow::paintStatusBar()
{
  struct IntuiText intuiText;
  intuiText.FrontPen  = m_AppScreen.Pens().Text();
  intuiText.BackPen   = m_AppScreen.Pens().Background();
  intuiText.DrawMode  = JAM2;
  intuiText.ITextFont = &m_TextAttr;
  intuiText.NextText  = NULL;

  int top = m_TextAreasTop + m_TextAreasHeight + 2;
  int left = m_TextArea1Left + 2;

  intuiText.TopEdge   = top;
  intuiText.LeftEdge  = left;
  intuiText.IText = (UBYTE*)
    SimpleString(m_StatusBarText + "   |   ").C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText);
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AppScreen.Pens().Green();
  intuiText.IText = (UBYTE*) m_AddedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AppScreen.Pens().Yellow();
  intuiText.IText = (UBYTE*) m_ChangedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  left += IntuiTextLength(&intuiText) + 5;
  intuiText.LeftEdge = left;
  intuiText.BackPen = m_AppScreen.Pens().Red();
  intuiText.IText = (UBYTE*) m_DeletedText.C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);
}

LONG DiffWindow::colorNameToPen(DiffDocument::ColorName colorName)
{
  if(colorName == DiffDocument::CN_Green)
  {
    return m_AppScreen.Pens().Green();
  }
  else if(colorName== DiffDocument::CN_Yellow)
  {
    return m_AppScreen.Pens().Yellow();
  }
  else if(colorName == DiffDocument::CN_Red)
  {
    return m_AppScreen.Pens().Red();
  }
  else
  {
    return m_AppScreen.Pens().Background();
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
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move each text area right by n * the height of one text line
  ScrollRaster(m_pWindow->RPort,
    -numChars * m_TextFontWidth_pix, // n * width
    0,
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  ScrollRaster(m_pWindow->RPort,
    -numChars * m_TextFontWidth_pix,  // n * width
    0,
    m_TextArea2Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  // fill the gap with the previous chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, pRightLine, (i - m_Y) * m_TextFontHeight_pix,
      m_X - numChars, numChars);
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
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move each text area left by n * the width of one char
  ScrollRaster(m_pWindow->RPort,
    numChars * m_TextFontWidth_pix,
    0,
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  ScrollRaster(m_pWindow->RPort,
    numChars * m_TextFontWidth_pix,
    0,
    m_TextArea2Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  // Fill the gap with the following chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(i);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, pRightLine, (i - m_Y)  * m_TextFontHeight_pix,
      m_X + m_MaxTextAreaChars, -numChars);
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
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move each text area downward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    -numLines * m_TextFontHeight_pix,  // n * height
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  ScrollRaster(m_pWindow->RPort, 0,
    -numLines * m_TextFontHeight_pix,  // n * height
    m_TextArea2Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  // fill the gap with the previous text lines
  for(int i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pLeftDocument->GetIndexedLine(lineIndex);
    const DiffLine* pRightLine = m_pRightDocument->GetIndexedLine(lineIndex);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, pRightLine, i * m_TextFontHeight_pix);
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

  if(m_pLeftDocument->NumLines() < m_MaxTextAreaLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_MaxTextAreaLines) == m_pLeftDocument->NumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_MaxTextAreaLines + numLines) > m_pLeftDocument->NumLines())
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    numLines = m_pLeftDocument->NumLines() - (m_Y + m_MaxTextAreaLines);
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move each text area upward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    numLines * m_TextFontHeight_pix,
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

  ScrollRaster(m_pWindow->RPort, 0,
    numLines * m_TextFontHeight_pix,
    m_TextArea2Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 3);

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
    paintLine(pLeftLine, pRightLine, paintLineIndex * m_TextFontHeight_pix);
  }

  // Repaint window decoration
  //paintWindowDecoration();

  return numLines;
}
