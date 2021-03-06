#include <string.h>

#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include "TextWindow.h"

extern struct GfxBase* GfxBase;

TextWindow::TextWindow(AppScreen& p_AppScreen, 
                       struct MsgPort* p_pMsgPort, 
                       int& p_NumWindowsOpen)
  : ScrollbarWindow(p_AppScreen, p_pMsgPort, p_NumWindowsOpen),
    m_pDocument(NULL),
    m_TextFontWidth_pix(0),
    m_TextFontHeight_pix(0),
    m_Y(0),
    m_X(0),
    m_MaxWinChars(0),
    m_MaxWinLines(0),
    m_IndentX(5),
    m_IndentY(5),
    m_TextAreaLeft(0),
    m_TextAreaTop(0),
    m_TextAreaWidth(0),
    m_TextAreaHeight(0)
{

}

TextWindow::~TextWindow()
{
  close();
}


void TextWindow::performResize()
{
  if(!isOpen())
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
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  if(m_pDocument != NULL)
  {
    paintDocument();
  }
}

bool TextWindow::Open(const APTR p_pMenuItemDisableAtOpen)
{
  if(ScrollbarWindow::open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  //
  // Calculate some initial values which only have to be calculated
  // once after window opening
  //
  m_TextAreaLeft = m_IndentX;
  m_TextAreaTop = m_IndentY;

  // Get the width and height of a char of the default rastport font
  // (This is the systems default monospace font)
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

  return true;
}


bool TextWindow::setDocument(TextDocument* p_pTextDocument)
{
  if(p_pTextDocument == NULL)
  {
    return false;
  }

  m_pDocument = p_pTextDocument;
  m_X = 0;
  m_Y = 0;

  if(!isOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Set full path of opened file as window title
  setTitle(p_pTextDocument->FileName());

  // Display the first [1; m_MaxWinLines] lines
  paintDocument();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  setYScrollPotSize(m_MaxWinLines, m_pDocument->getNumLines());

  return true;
}

void TextWindow::XChangedHandler(size_t p_NewY)
{
  // TODO
}

void TextWindow::handleYChange(size_t p_NewY)
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
  // Scroll bigger amounts by re-painting the whole page at the new
  // y-position
  //
  m_Y = p_NewY;

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Display the beginning at new y-position
  paintDocument(true);
}

void TextWindow::handleXIncrease(size_t p_IncreaseBy,
  bool p_bTriggeredByScrollbarPot)
{
  m_X += scrollNCharsLeft(p_IncreaseBy);

  if(!p_bTriggeredByScrollbarPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollTop(m_X);
  }
}

void TextWindow::handleXDecrease(size_t p_DecreaseBy,
  bool p_bTriggeredByScrollbarPot)
{
  m_X -= scrollNCharsRight(p_DecreaseBy);

  if(!p_bTriggeredByScrollbarPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setXScrollTop(m_X);
  }
}


void TextWindow::handleYIncrease(size_t p_IncreaseBy,
  bool p_bTriggeredByScrollbarPot)
{
  m_Y += scrollNLinesUp(p_IncreaseBy);

  if(!p_bTriggeredByScrollbarPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_Y);
  }
}


void TextWindow::handleYDecrease(size_t p_DecreaseBy,
  bool p_bTriggeredByScrollbarPot)
{
  m_Y -= scrollNLinesDown(p_DecreaseBy);

  if(!p_bTriggeredByScrollbarPot)
  {
    // Y-position-decrease was not triggered by the scrollbar pot
    // directly. So the pot top position must be set manually.
    setYScrollTop(m_Y);
  }
}


void TextWindow::initialize()
{
  // Call parent method to get to utilize scroll gadgets iside the
  // window borders
  ScrollbarWindow::initialize();

  // Set the default title
  setTitle("TextWindow");

  // Setting the window flags
  addFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DEPTHGADGET |     // Add a depth gadget
           WFLG_SIZEGADGET |      // Add a size gadget
           WFLG_GIMMEZEROZERO);   // Different layers for border and content

  // Setting the IDCMP messages we want to receive for this window
  addIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_CLOSEWINDOW |    // Inform us about click on close gadget
           IDCMP_NEWSIZE);        // Inform us about resizing

  m_bInitialized = true;
}


bool TextWindow::handleIDCMP(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(ScrollbarWindow::handleIDCMP(p_Class, p_Code, p_IAddress) == true)
  {
    return true;
  }

  switch (p_Class)
  {
    case IDCMP_NEWSIZE:
    {
      performResize();
      return true;
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      close();
      return true;
      break;
    }
  }

  return false;
}

void TextWindow::calcSizes()
{
  // (Re-)calculate some values that may have be changed by re-sizing
  ScrollbarWindow::calcSizes();

  m_TextAreaWidth = m_InnerWindowRight - m_TextAreaLeft - m_IndentX;
  m_TextAreaHeight = m_InnerWindowBottom - m_TextAreaTop - m_IndentY;

  if((m_TextFontHeight_pix == 0) || (m_TextFontWidth_pix == 0))
  {
    return;
  }

  // Calculate how many text lines fit in window
  m_MaxWinLines = (m_TextAreaHeight - 4) /  m_TextFontHeight_pix;

  // Calculate how many chars fit on each line
  m_MaxWinChars = (m_TextAreaWidth - 4) / m_TextFontWidth_pix;

  // Set y-scroll-gadget's pot size in relation of new window size
  setYScrollPotSize(m_MaxWinLines);
}


void TextWindow::paintLine(const std::string* p_pLine, WORD p_TopEdge)
{
  // Move rastport cursor to start of left line
  ::Move(m_pWindow->RPort,
    m_TextAreaLeft + 3,
    p_TopEdge + m_TextAreaTop + m_TextFontHeight_pix
  );

  // Determine how many characters would be print theoretically
  size_t numChars = p_pLine->Length();

  // Limit this number to the max fitting chars if exceeded
  numChars = numChars > m_MaxWinChars ? m_MaxWinChars : numChars;

  // Print the line
  Text(m_pWindow->RPort,
    p_pLine->C_str(),
    numChars
  );
}

void TextWindow::paintDocument(bool p_bStartFromTop)
{
  if(m_pDocument == NULL)
  {
    return;
  }

  if(p_bStartFromTop == true)
  {
    m_Y = 0;
  }

  for(int i = m_Y; (i - m_Y) < m_MaxWinLines; i++)
  {
    if(i >= m_pDocument->getNumLines())
    {
      break;
    }

    const std::string* pLine = m_pDocument->GetIndexedLine(i);

    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine, (i - m_Y) * m_TextFontHeight_pix);
  }
}

size_t TextWindow::scrollNCharsRight(int p_ScrollNumCharsRight)
{
  if(p_ScrollNumCharsRight < 1)
  {
    // Nothing to do
    return 0;
  }

  if(m_Y < 1)
  {
    // Do not move the text area right if text is already at leftmost position
    return 0;
  }

  if(p_ScrollNumCharsRight > m_X)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    p_ScrollNumCharsRight = m_X;
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move the text area right by n * the height of one text line
  ScrollRaster(m_pWindow->RPort,
    -p_ScrollNumCharsRight * m_TextFontWidth_pix, // n * width
    0,
    m_TextAreaLeft + 3, m_TextAreaTop + 2,
    m_TextAreaLeft + m_TextAreaWidth - 3,
    m_TextAreaTop + m_TextAreaHeight - 3);


  // fill the gap with the previous chars
  for(int i = m_Y; i < m_Y + m_MaxWinLines; i++)
  {
    const std::string* pLine = m_pDocument->GetIndexedLine(i);

    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine, i * m_TextFontHeight_pix);
  }

  return p_ScrollNumCharsRight;
}

size_t TextWindow::scrollNCharsLeft(int p_ScrollNumCharsLeft)
{
  if(p_ScrollNumCharsLeft < 1)
  {
    // Noting to do
    return 0;
  }

  if(m_pDocument->MaxLineLength() < m_MaxWinChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxWinChars) == m_pDocument->getNumLines())
  {
    // Do not move the scroll area left if text already at rightmost position
    return 0;
  }

  if((m_X + m_MaxWinChars + p_ScrollNumCharsLeft) > m_pDocument->MaxLineLength())
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    p_ScrollNumCharsLeft = m_pDocument->MaxLineLength() - (m_X + m_MaxWinChars);
  }

  // Set foreground color for document painting
  SetAPen(m_pWindow->RPort, m_AppScreen.Pens().Text());

  // Set background color before scrolling
  SetBPen(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Move each text area left by n * the width of one char
  ScrollRaster(m_pWindow->RPort,
    p_ScrollNumCharsLeft * m_TextFontWidth_pix,
    0,
    m_TextAreaLeft + 3, m_TextAreaTop + 2,
    m_TextAreaLeft + m_TextAreaWidth - 3,
    m_TextAreaTop + m_TextAreaHeight - 3);


  // Fill the gap with the following chars
  for(int i = m_Y; i < m_Y + m_MaxWinLines; i++)
  {
    const std::string* pLine = m_pDocument->GetIndexedLine(i);

    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine, i * m_TextFontHeight_pix);
  }

  return p_ScrollNumCharsLeft;
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
    -p_ScrollNumLinesDown * m_TextFontHeight_pix,  // n * height
    m_TextAreaLeft, m_TextAreaTop,
    m_TextAreaLeft + m_TextAreaWidth,
    m_TextAreaTop + m_TextAreaHeight);

  // fill the gap with the previous text lines
  for(int i = 0; i < p_ScrollNumLinesDown; i++)
  {
    int lineIndex = m_Y - p_ScrollNumLinesDown + i;
    const std::string* pLine = m_pDocument->GetIndexedLine(lineIndex);

    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine, i * m_TextFontHeight_pix);
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

  if(m_pDocument->getNumLines() < m_MaxWinLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_MaxWinLines) == m_pDocument->getNumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_MaxWinLines + p_ScrollUpNumLinesUp) > m_pDocument->getNumLines())
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    p_ScrollUpNumLinesUp = m_pDocument->getNumLines() - (m_Y + m_MaxWinLines);
  }

  // Move text area upward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    p_ScrollUpNumLinesUp * m_TextFontHeight_pix,
    m_TextAreaLeft, m_TextAreaTop,
    m_TextAreaLeft + m_TextAreaWidth,
    m_TextAreaTop + m_TextAreaHeight);

  for(int i = 0; i < p_ScrollUpNumLinesUp; i++)
  {
    int lineIndex = m_Y + m_MaxWinLines + i;
    const std::string* pLine = m_pDocument->GetIndexedLine(lineIndex);

    if(pLine == NULL)
    {
      break;
    }

    int paintLineIndex = m_MaxWinLines - p_ScrollUpNumLinesUp + i;
    paintLine(pLine, paintLineIndex * m_TextFontHeight_pix);
  }

  return p_ScrollUpNumLinesUp;
}
