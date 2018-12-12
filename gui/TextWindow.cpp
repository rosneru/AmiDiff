#include <string.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include "TextWindow.h"

TextWindow::TextWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort)
  : ScrollbarWindow(p_AppScreen, p_pMsgPort),
    m_pDocument(NULL),
    m_MaxWinLines(0),
    m_Y(0),
    m_TextAreaLeft(0),
    m_TextAreaTop(0),
    m_TextAreaRight(0),
    m_TextAreaBottom(0)
{

}

TextWindow::~TextWindow()
{
  Close();
}


void TextWindow::Resized()
{
  if(m_pDocument == NULL)
  {
    return; // TODO
  }

  // Calculate how many lines *now* can be displayed in the window
  calcSizes();

  if(m_pDocument->NumLines() == 0)
  {
    return;
  }

  // Set scroll gadgets pot size in relation of new window size
  setYScrollPotSize(m_MaxWinLines);

  // Redraw obscured window regions
  Refresh();
}

void TextWindow::Refresh()
{
  BeginRefresh(m_pWindow);
  paintDocument();
  EndRefresh(m_pWindow, TRUE);
}

bool TextWindow::Open(APTR p_pMenuItemDisableAtOpen)
{
  if(ScrollbarWindow::Open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  // Calculate values needed for text scrolling
  m_TextAreaRight = m_pWindow->Width; //- m_pWindow->BorderRight - m_TextAreaLeft;
  m_TextAreaBottom = m_pWindow->Height; // - m_pWindow->BorderBottom - m_TextAreaTop;

  // Calculate how many lines can be displayed in the window
  calcSizes();

  m_TextAttr.ta_Name = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Flags;

  // Prepare IntuiText for line-by-line printing
  m_IntuiText.DrawMode  = JAM2;
  m_IntuiText.LeftEdge  = 0;
  m_IntuiText.TopEdge   = 0;
  m_IntuiText.ITextFont = &m_TextAttr;
  m_IntuiText.NextText  = NULL;

  return true;
}


bool TextWindow::SetContent(TextDocument* p_pTextDocument)
{
  m_pDocument = p_pTextDocument;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Set full path of opened file as window title
  SetTitle(p_pTextDocument->FileName());

  // Display the first [1; m_MaxWinLines] lines
  paintDocument();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxWinLines, m_pDocument->NumLines());

  return true;
}

void TextWindow::YChangedHandler(size_t p_NewY)
{
  int delta = p_NewY - m_Y;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);
  int deltaLimit = m_MaxWinLines / 2;

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
  int numListTraverses = delta - m_MaxWinLines + 1;

  // Depending on numListTraverses call getPrev and getNext as often
  // as needed
  if(numListTraverses < 0)
  {
    for(int i = 0; i < -numListTraverses; i++)
    {
      m_pDocument->GetPreviousLine();
    }
  }
  else if(numListTraverses > 0)
  {
    for(int i = 0; i < numListTraverses; i++)
    {
      m_pDocument->GetNextLine();
    }
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Display the beginning at new y-position
  paintDocument(true);
}


void TextWindow::YIncrease(size_t p_IncreaseBy,
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


void TextWindow::YDecrease(size_t p_DecreaseBy,
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


void TextWindow::initialize()
{
  // Call parent method to get to utilize scroll gadgets iside the
  // window borders
  ScrollbarWindow::initialize();

  // Set the default title
  SetTitle("TextWindow");

  // Setting the window flags
  setFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DEPTHGADGET |     // Add a depth gadget
           WFLG_SIZEGADGET |      // Add a size gadget
           WFLG_GIMMEZEROZERO);   // Different layers for border and content

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_CLOSEWINDOW |    // Inform us about click on close gadget
           IDCMP_NEWSIZE |        // Inform us about resizing
           IDCMP_IDCMPUPDATE);    // Inform us about TODO

  m_bInitialized = true;
}


bool TextWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
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

    case IDCMP_REFRESHWINDOW:
    {
      Refresh();
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

void TextWindow::calcSizes()
{
  if(m_AppScreen.FontHeight() == 0)
  {
    return;
  }

  m_MaxWinLines = m_pWindow->Height;
  m_MaxWinLines -= m_pWindow->BorderTop;
  m_MaxWinLines -= m_pWindow->BorderBottom;
  m_MaxWinLines -= m_TextAreaTop;
  m_MaxWinLines /= m_AppScreen.FontHeight();
}


void TextWindow::paintLine(const SimpleString* p_pLine, WORD p_TopEdge)
{
  m_IntuiText.IText = (UBYTE*)p_pLine->C_str();
  m_IntuiText.TopEdge = p_TopEdge;
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_TextAreaLeft, m_TextAreaTop);
}

void TextWindow::paintDocument(bool p_bStartFromCurrentY)
{
  if(m_pDocument == NULL)
  {
    return;
  }

  size_t i = m_Y;
  const SimpleString* pLine = NULL;
  const SimpleString* pRightLine = NULL;

  if(p_bStartFromCurrentY == true)
  {
    pLine = m_pDocument->GetCurrentLine();
  }
  else
  {
    pLine = m_pDocument->GetIndexedLine(i);
  }

  while((pLine != NULL) && (pRightLine !=NULL))
  {
    int lineNum = i - m_Y;

    paintLine(pLine, lineNum * m_AppScreen.FontHeight());

    if(lineNum >= m_MaxWinLines - 1)
    {
      // Only display as many lines as fit into the window
      break;
    }

    i++;

    pLine = m_pDocument->GetNextLine();
  }
}

size_t TextWindow::scrollNLinesDown(int p_ScrollNumLinesDown)
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
    m_TextAreaLeft, m_TextAreaTop,
    m_TextAreaRight, m_TextAreaBottom);

  // This id only is used in the first call of
  // GetPreviousOrIndexedLine() in the loop below. The next calls don't
  // use the index, instead they use GetPrevious(). Because of this it
  // is no problem that weather the index itself nor m_Y etc are
  // updated in the loop.
  int previousLineId = m_Y - 1;

  // fill the gap with the previous text lines
  for(int i = 0; i < p_ScrollNumLinesDown; i++)
  {
    const SimpleString* pLine = NULL;

    pLine = m_pDocument->GetPreviousOrIndexedLine(previousLineId);

    if(pLine == NULL)
    {
      break;
    }

    int lineNum = p_ScrollNumLinesDown - i - 1;

    paintLine(pLine, lineNum * m_AppScreen.FontHeight());
  }

  return p_ScrollNumLinesDown;
}

size_t TextWindow::scrollNLinesUp(int p_ScrollUpNumLinesUp)
{
  if(p_ScrollUpNumLinesUp < 1)
  {
    // Noting to do
    return 0;
  }

  if(m_pDocument->NumLines() < m_MaxWinLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_MaxWinLines) == m_pDocument->NumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_MaxWinLines + p_ScrollUpNumLinesUp) > m_pDocument->NumLines())
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    p_ScrollUpNumLinesUp = m_pDocument->NumLines() - (m_Y + m_MaxWinLines);
  }

  // Move text area upward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    p_ScrollUpNumLinesUp * m_AppScreen.FontHeight(),
    m_TextAreaLeft, m_TextAreaTop,
    m_TextAreaRight, m_TextAreaBottom);

  // This id only is used in the first call of GetNextOrIndexedLine()
  // in the loop below. The next calls don't use the index, instead
  // they use GetNext(). Because of this it is no problem that weather
  // the index itself nor m_Y etc are updated in the loop.
  int nextLineId = m_Y + m_MaxWinLines;

  for(int i = 0; i < p_ScrollUpNumLinesUp; i++)
  {
    const SimpleString* pLine = NULL;
    pLine = m_pDocument->GetNextOrIndexedLine(nextLineId);

    if(pLine == NULL)
    {
      break;
    }

    int lineNum = m_MaxWinLines - p_ScrollUpNumLinesUp + i;

    paintLine(pLine, lineNum * m_AppScreen.FontHeight());
  }

  return p_ScrollUpNumLinesUp;
}
