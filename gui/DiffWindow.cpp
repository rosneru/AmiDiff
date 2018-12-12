#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <clib/alib_protos.h>
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

extern struct Library* GfxBase;

DiffWindow::DiffWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort)
  : ScrollbarWindow(p_AppScreen, p_pMsgPort),
    m_pLeftDocument(NULL),
    m_pRightDocument(NULL),
    m_TextFontWidth_pix(0),
    m_TextFontHeight_pix(0),
    m_X(0),
    m_Y(0),
    m_MaxTextAreaLines(0),
    m_MaxTextAreaChars(0),
    m_IndentX(5),
    m_IndentY(0),
    m_TextArea1Left(0),
    m_TextArea2Left(0),
    m_TextAreasTop(0),
    m_TextAreasWidth(0),
    m_TextAreasHeight(0),
    m_LineNumbersWidth(0)
{

}

DiffWindow::~DiffWindow()
{
  Close();
}


void DiffWindow::Resized()
{
  if(!IsOpen())
  {
    return; // TODO
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

  if((m_pLeftDocument != NULL) && m_pRightDocument != NULL)
  {
    // Paint the document names
    paintDocumentNames();

    // Paint the content of the two documents
    paintDocument();
  }

  // Paint the window decoration
  paintWindowDecoration();

}

bool DiffWindow::Open(APTR p_pMenuItemDisableAtOpen)
{
  if(ScrollbarWindow::Open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  //
  // Calculate some initial values which only have to be calculated
  // once after window opening
  //
  m_IndentY = 2 * m_AppScreen.FontHeight();
  m_TextArea1Left = m_IndentX;
  m_TextAreasTop = m_IndentY;

  struct TextFont* defaultTextFont = ((struct GfxBase *)GfxBase)->DefaultFont;
  m_TextFontWidth_pix = defaultTextFont->tf_XSize;
  m_TextFontHeight_pix = defaultTextFont->tf_YSize;


  // Calculate some values which have to calculated after window
  // opening and after resizing
  calcSizes();

  m_TextAttr.ta_Name = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Flags;

  // Paint the window decoration
  paintWindowDecoration();

  return true;
}

bool DiffWindow::SetContent(DiffDocument* p_pLeftDocument,
  DiffDocument* p_pRightDocument)
{
  if((p_pLeftDocument == NULL) || (p_pRightDocument == NULL))
  {
    return false;
  }

  m_pLeftDocument = p_pLeftDocument;
  m_pRightDocument = p_pRightDocument;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // TODO Instead 9999 use some algorithm to use exactly as many 9s as
  //      the m_LeftDocument.NumLines digits has. Maybe sprintf numLines
  //      to a buf and use this?
  m_LineNumbersWidth = TextLength(m_pWindow->RPort, "9999", 4);


  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Display the document titles above the text areas
  paintDocumentNames();

  // Display the first [1; m_MaxTextAreaLines] lines
  paintDocument();

  // Paint the window decoration
  paintWindowDecoration();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxTextAreaLines, m_pLeftDocument->NumLines());

  return true;
}

void DiffWindow::YChangedHandler(size_t p_NewY)
{
  int delta = p_NewY - m_Y;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);
  int deltaLimit = m_MaxTextAreaLines / 2;

  //
  // Scroll small amounts (1/2 window height) line by line
  //
  if(deltaAbs < deltaLimit)
  {
    if(delta > 0)
    {
      m_Y += scrollNLinesUp(deltaAbs);
    }
    else if(delta < 0)
    {
      m_Y -= scrollNLinesDown(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-paintting the whole page at the new
  // y-position
  //
  m_Y = p_NewY;

  // Determine how often getPrev or getNext must be called to set the
  // left and right documents to the new start point for page redrawing
  int numListTraverses = delta - m_MaxTextAreaLines + 1;

  // Depending on numListTraverses call getPrev and getNext as often
  // as needed
  if(numListTraverses < 0)
  {
    for(int i = 0; i < -numListTraverses; i++)
    {
      m_pLeftDocument->GetPreviousLine();
      m_pRightDocument->GetPreviousLine();
    }
  }
  else if(numListTraverses > 0)
  {
    for(int i = 0; i < numListTraverses; i++)
    {
      m_pLeftDocument->GetNextLine();
      m_pRightDocument->GetNextLine();
    }
  }

  // Clear both text areas completely
  EraseRect(m_pWindow->RPort,
    m_TextArea1Left + 2, m_TextAreasTop + 2,
    m_TextArea1Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 4);

  EraseRect(m_pWindow->RPort,
    m_TextArea2Left + 2, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 4);

  paintDocument(true);

  paintWindowDecoration();

}

void DiffWindow::YIncrease(size_t p_IncreaseBy,
  bool p_bTriggeredByArrowGadget)
{
  m_Y += scrollNLinesUp(p_IncreaseBy);

  if(!p_bTriggeredByArrowGadget)
  {
    // Y-position-decrease was not triggered by the up-arrow-gadget:
    // Manually det the new TOP value for the y-scrollbar
    setYScrollTop(m_Y);
  }
}

void DiffWindow::YDecrease(size_t p_DecreaseBy,
  bool p_bTriggeredByArrowGadget)
{
  m_Y -= scrollNLinesDown(p_DecreaseBy);

  if(!p_bTriggeredByArrowGadget)
  {
    // Y-position-decrease was not triggered by the up-arrow-gadget:
    // Manually det the new TOP value for the y-scrollbar
    setYScrollTop(m_Y);
  }
}

void DiffWindow::initialize()
{
  // Call parent method to get to utilize scroll gadgets iside the
  // window borders
  ScrollbarWindow::initialize();

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
           IDCMP_NEWSIZE);    // Inform us about TODO

  m_bInitialized = true;

}

bool DiffWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(ScrollbarWindow::HandleIdcmp(p_Class, p_Code, p_IAddress) == true)
  {
    return true;
  }

  switch (p_Class)
  {
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



void DiffWindow::calcSizes()
{
  // (Re-)calculate some values that may have be changed by re-sizing
  ScrollbarWindow::calcSizes();

  m_TextAreasWidth = m_InnerWindowRight - m_TextArea1Left - m_IndentX;
  m_TextAreasWidth /= 2;

  m_TextAreasHeight = m_InnerWindowBottom - m_TextAreasTop - m_IndentY;

  m_TextArea2Left = m_TextArea1Left + m_TextAreasWidth;

  if(m_AppScreen.FontHeight() == 0)
  {
    return;
  }

  // Calculates how many lines fit into each text area
  m_MaxTextAreaLines = (m_TextAreasHeight - 4) /  m_AppScreen.FontHeight();

  // Calculates how many chars fit on each line in each text area
  m_MaxTextAreaChars = (m_TextAreasWidth - 4) / m_TextFontWidth_pix;

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_MaxTextAreaLines);
}

void DiffWindow::paintDocument(bool p_bStartFromCurrentY)
{
  if(m_pLeftDocument == NULL || m_pRightDocument == NULL)
  {
    return; // TODO
  }

  size_t i = m_Y;
  const SimpleString* pLeftLine = NULL;
  const SimpleString* pRightLine = NULL;

  if(p_bStartFromCurrentY == true)
  {
    pLeftLine = m_pLeftDocument->GetCurrentLine();
    pRightLine = m_pRightDocument->GetCurrentLine();
  }
  else
  {
    pLeftLine = m_pLeftDocument->GetIndexedLine(i);
    pRightLine = m_pRightDocument->GetIndexedLine(i);
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  while((pLeftLine != NULL) && (pRightLine !=NULL))
  {
    int lineNum = i - m_Y;

    paintLine(pLeftLine, pRightLine,
      lineNum * m_AppScreen.FontHeight());

    if(lineNum >= m_MaxTextAreaLines - 1)
    {
      // Only display as many lines as fit into the window
      break;
    }

    i++;

    pLeftLine = m_pLeftDocument->GetNextLine();
    pRightLine = m_pRightDocument->GetNextLine();
  }
}

void DiffWindow::paintLine(const SimpleString* p_pLeftLine,
    const SimpleString* p_pRightLine, WORD p_TopEdge)
{
  // Move rastport cursor to start of left line
  ::Move(m_pWindow->RPort,
    m_TextArea1Left + 3,
    p_TopEdge + m_TextAreasTop + m_TextFontHeight_pix
  );

  // Set the left line's background color
  SetBPen(m_pWindow->RPort, colorNameToPen(m_pLeftDocument->LineColor()));

  // Determine how many characters would be print theoretically
  size_t numChars = p_pLeftLine->Length();

  // Limit this number to the max fitting chars if exceeded
  numChars = numChars > m_MaxTextAreaChars ? m_MaxTextAreaChars : numChars;

  Text(m_pWindow->RPort,
    p_pLeftLine->C_str(), // TODO (UBYTE*) or (STRPTR)
    numChars
  );

  // Move rastport cursor to start of right line
  ::Move(m_pWindow->RPort,
    m_TextArea2Left + 3,
    p_TopEdge + m_TextAreasTop + m_TextFontHeight_pix
  );

  // Set the right line's background color
  SetBPen(m_pWindow->RPort, colorNameToPen(m_pRightDocument->LineColor()));

  // Determine how many characters would be print theoretically
  numChars = p_pRightLine->Length();

  // Limit this number to the max fitting chars if exceeded
  numChars = numChars > m_MaxTextAreaChars ? m_MaxTextAreaChars : numChars;

  Text(m_pWindow->RPort,
    p_pRightLine->C_str(), // TODO (UBYTE*) or (STRPTR)
    numChars
  );
}

void DiffWindow::paintWindowDecoration()
{
  // Create borders for the two text areas
  DrawBevelBox(m_pWindow->RPort,
    m_TextArea1Left, m_TextAreasTop,
    m_TextAreasWidth, m_TextAreasHeight,
    GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);

  DrawBevelBox(m_pWindow->RPort,
    m_TextArea2Left, m_TextAreasTop,
    m_TextAreasWidth, m_TextAreasHeight,
    GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);
}

void DiffWindow::paintDocumentNames()
{
  if((m_pLeftDocument == NULL) || (m_pRightDocument == NULL))
  {
    return;
  }

  struct IntuiText intuiText;
  intuiText.FrontPen  = m_AppScreen.Pens().HighlightedText();
  intuiText.BackPen   = m_AppScreen.Pens().Background();
  intuiText.DrawMode  = JAM2;
  intuiText.ITextFont = &m_TextAttr;
  intuiText.NextText  = NULL;

  intuiText.TopEdge   = m_TextAreasTop - m_AppScreen.FontHeight() - 2;

  intuiText.LeftEdge  = m_TextArea1Left + 2;
  intuiText.IText = (UBYTE*)m_pLeftDocument->FileName().C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);

  intuiText.LeftEdge  = m_TextArea2Left + 2;
  intuiText.IText = (UBYTE*)m_pRightDocument->FileName().C_str();
  PrintIText(m_pWindow->RPort, &intuiText, 0, 0);
}

LONG DiffWindow::colorNameToPen(DiffDocument::ColorName p_pColorName)
{
  if(p_pColorName == DiffDocument::CN_Green)
  {
    return m_AppScreen.Pens().Green();
  }
  else if(p_pColorName== DiffDocument::CN_Yellow)
  {
    return m_AppScreen.Pens().Yellow();
  }
  else if(p_pColorName == DiffDocument::CN_Red)
  {
    return m_AppScreen.Pens().Red();
  }
  else
  {
    return m_AppScreen.Pens().Background();
  }
}

size_t DiffWindow::scrollNLinesDown(int p_ScrollNumLinesDown)
{
  if(p_ScrollNumLinesDown < 1)
  {
    // Nothing to do
    return 0;
  }

  if(m_Y < 1)
  {
    // Do not move the text area downward if text is already at top
    return 0;
  }

  if(p_ScrollNumLinesDown > m_Y)
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    p_ScrollNumLinesDown = m_Y;
  }

  // Move text area downward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    -p_ScrollNumLinesDown * m_AppScreen.FontHeight(),  // n * height
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 2);

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // This id only is used in the first call of
  // GetPreviousOrIndexedLine() in the loop below. The next calls don't
  // use the index, instead they use GetPrevious(). Because of this it
  // is no problem that weather the index itself nor m_Y etc are
  // updated in the loop.
  int previousLineId = m_Y - 1;

  // fill the gap with the previous text lines
  for(int i = 0; i < p_ScrollNumLinesDown; i++)
  {
    const SimpleString* pLeftLine = NULL;
    const SimpleString* pRightLine = NULL;

    pLeftLine = m_pLeftDocument->GetPreviousOrIndexedLine(previousLineId);
    pRightLine = m_pRightDocument->GetPreviousOrIndexedLine(previousLineId);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    int lineNum = p_ScrollNumLinesDown - i - 1;

    paintLine(pLeftLine, pRightLine, lineNum * m_AppScreen.FontHeight());
  }

  // Repaint window decoration
  paintWindowDecoration();

  return p_ScrollNumLinesDown;
}

size_t DiffWindow::scrollNLinesUp(int p_ScrollUpNumLinesUp)
{
  if(p_ScrollUpNumLinesUp < 1)
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

  if((m_Y + m_MaxTextAreaLines + p_ScrollUpNumLinesUp) > m_pLeftDocument->NumLines())
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    p_ScrollUpNumLinesUp = m_pLeftDocument->NumLines() - (m_Y + m_MaxTextAreaLines);
  }

  // Move text area upward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    p_ScrollUpNumLinesUp * m_AppScreen.FontHeight(),
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 2);

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // This id only is used in the first call of GetNextOrIndexedLine()
  // in the loop below. The next calls don't use the index, instead
  // they use GetNext(). Because of this it is no problem that weather
  // the index itself nor m_Y etc are updated in the loop.
  int nextLineId = m_Y + m_MaxTextAreaLines;

  for(int i = 0; i < p_ScrollUpNumLinesUp; i++)
  {
    const SimpleString* pLeftLine = NULL;
    const SimpleString* pRightLine = NULL;


    pLeftLine = m_pLeftDocument->GetNextOrIndexedLine(nextLineId);
    pRightLine = m_pRightDocument->GetNextOrIndexedLine(nextLineId);

    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    int lineNum = m_MaxTextAreaLines - p_ScrollUpNumLinesUp + i;

    paintLine(pLeftLine, pRightLine,
      lineNum * m_AppScreen.FontHeight());
  }

  // Repaint window decoration
  paintWindowDecoration();

  return p_ScrollUpNumLinesUp;
}
