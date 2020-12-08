#ifdef __clang__
  #include <clib/graphics_protos.h>
#else
  #include <proto/graphics.h>
#endif

#include <math.h>
#include <string.h>

#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(const DiffOutputFileBase* pDiffFile,
                                       DiffWindowRastports*& pRPorts,
                                       struct TextFont* pTextFont,
                                       bool lineNumbersEnabled,
                                       ULONG maxNumChars)
  : m_pDiffFile(pDiffFile),
    m_pRPorts(pRPorts),
    m_LineNumbersEnabled(lineNumbersEnabled),
    m_MaxNumChars(maxNumChars),
    m_NumLines(pDiffFile->NumLines()),
    m_FontWidth_pix(pTextFont->tf_XSize),
    m_FontHeight_pix(pTextFont->tf_YSize),
    m_FontBaseline_pix(pTextFont->tf_Baseline),
    m_MaxVisibleChars(0),
    m_MaxVisibleLines(0),
    m_X(0),
    m_Y(0),
    m_LineNumsWidth_chars(0),
    m_LineNumsWidth_pix(0)
{
  if(lineNumbersEnabled)
  {
    const DiffLine* pLine = pDiffFile->GetLine(0);
    const char* pLineNum = pLine->LineNum();

    m_LineNumsWidth_chars = strlen(pLineNum);
    m_LineNumsWidth_pix = m_LineNumsWidth_chars * m_FontWidth_pix;
  }
}


DiffWindowTextArea::~DiffWindowTextArea()
{

}


ULONG DiffWindowTextArea::X() const
{
  return m_X;
}


ULONG DiffWindowTextArea::Y() const
{
  return m_Y;
}


ULONG DiffWindowTextArea::MaxVisibleChars() const
{
  return m_MaxVisibleChars;
}


ULONG DiffWindowTextArea::MaxVisibleLines() const
{
  return m_MaxVisibleLines;
}


void DiffWindowTextArea::SetWidthHeight(unsigned long width,
                                        unsigned long height)
{

  m_MaxVisibleLines = (height - 4) /  m_FontHeight_pix;
  m_MaxVisibleChars = (width - 7 - m_LineNumsWidth_pix) / m_FontWidth_pix;

  // Limit height to int multiples
  height = m_MaxVisibleLines * m_FontHeight_pix + 3;

  Rect::SetWidthHeight(width, height);

  //
  // Define two the scrolling areas. Note that there are two different
  // scroll rects used because:
  //   The 'Left' values depend on if the scroll is done horizontally or
  //   vertically. On vertical scroll the block containing line numbers
  //   is scrolled too. On horizontal scroll, it is not.
  //

  ULONG maxTextWidth_pix = m_MaxVisibleChars * m_FontWidth_pix;
  maxTextWidth_pix += m_LineNumsWidth_pix;

  m_HScrollRect.Set(Left() + m_LineNumsWidth_pix + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);

  m_VScrollRect.Set(Left() + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);
}


void DiffWindowTextArea::Clear()
{
  // EraseRect(m_pRPorts->LineNum(),
  //           Left() + 2,
  //           Top() + 2,
  //           Right() - 3,
  //           Bottom() - 3);
}



void DiffWindowTextArea::ScrollTopToRow(ULONG rowId)
{
  // Prevent to scroll below the last line
  ULONG yLimit = m_NumLines - m_MaxVisibleLines;
  if((yLimit > 0) && ( rowId > yLimit))
  {
     rowId = yLimit;
  }

  int delta =  rowId - m_Y;
  if(delta == 0)
  {
    return;
  }

  int deltaAbs = abs(delta);

  int deltaLimit = m_MaxVisibleLines - 2;
  //int deltaLimit = 10;

  //
  // Scroll small amounts (1/2 text area height) line by line
  //
  if(deltaAbs < deltaLimit)
  {
    if(delta > 0)
    {
      ScrollUp(deltaAbs);
    }
    else if(delta < 0)
    {
      ScrollDown(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // y-position
  //
  m_Y =  rowId;

  EraseRect(m_pRPorts->Window(),
            Left() + 2,
            Top() + 2,
            Right() - 3,
            Bottom() - 3);

  PrintPage();
}


void DiffWindowTextArea::ScrollLeftToColumn(ULONG columId)
{
  long delta = columId - m_X;
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
      ScrollLeft(deltaAbs);
    }
    else if(delta < 0)
    {
      ScrollRight(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // x-position
  //
  m_X = columId;

  // Clear text area completely
  Clear();

  PrintPage();
}


ULONG DiffWindowTextArea::ScrollLeft(ULONG numChars)
{
  if(numChars < 1)
  {
    // Noting to do
    return 0;
  }

  if(numChars > m_MaxVisibleChars)
  {
    numChars = m_MaxVisibleChars;
  }

  if(m_MaxNumChars < m_MaxVisibleChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxVisibleChars) == m_MaxNumChars)
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_MaxVisibleChars + numChars) > m_MaxNumChars)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_MaxNumChars - (m_X + m_MaxVisibleChars);
  }

  // Move each text area left by n * the width of one char
  ScrollRasterBF(m_pRPorts->Window(),
                 numChars * m_FontWidth_pix,
                 0,
                 m_HScrollRect.Left(),
                 m_HScrollRect.Top(),
                 m_HScrollRect.Right(),
                 m_HScrollRect.Bottom());

  // Fill the gap with the following chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxVisibleLines; i++)
  {
    const DiffLine* pLine = m_pDiffFile->GetLine(i);

    if(pLine == NULL)
    {
      break;
    }

    drawDiffLine(pLine,
              (i - m_Y)  * m_FontHeight_pix,
              true,
              m_X + m_MaxVisibleChars,
              -numChars);

  }

  m_X += numChars;
  return numChars;
}


ULONG DiffWindowTextArea::ScrollRight(ULONG numChars)
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

  if(numChars > m_MaxVisibleChars)
  {
    numChars = m_MaxVisibleChars;
  }


  // Move each text area right by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 -numChars * m_FontWidth_pix, // n * width
                 0,
                 m_HScrollRect.Left(),
                 m_HScrollRect.Top(),
                 m_HScrollRect.Right(),
                 m_HScrollRect.Bottom());


  // fill the gap with the previous chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxVisibleLines; i++)
  {
    const DiffLine* pLine = m_pDiffFile->GetLine(i);
    if(pLine == NULL)
    {
      break;
    }

    drawDiffLine(pLine,
              (i - m_Y) * m_FontHeight_pix,
              true,
              m_X - numChars,
              numChars);

  }

  m_X -= numChars;
  return numChars;
}


ULONG DiffWindowTextArea::ScrollUp(ULONG numLines)
{
  if(numLines < 1)
  {
    // Noting to do
    return 0;
  }

  if(m_NumLines < m_MaxVisibleLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_MaxVisibleLines) == m_NumLines)
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_MaxVisibleLines + numLines) > m_NumLines)
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    numLines = m_NumLines - (m_Y + m_MaxVisibleLines);
  }

  // Move each text area upward by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 numLines * m_FontHeight_pix,
                 m_VScrollRect.Left(),
                 m_VScrollRect.Top(),
                 m_VScrollRect.Right(),
                 m_VScrollRect.Bottom());

  for(ULONG i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y + m_MaxVisibleLines + i;
    const DiffLine* pLine = m_pDiffFile->GetLine(lineIndex);
    if(pLine == NULL)
    {
      break;
    }

    int paintLineIndex = m_MaxVisibleLines - numLines + i;
    drawDiffLine(pLine,
              paintLineIndex * m_FontHeight_pix);
  }

  m_Y += numLines;
  return numLines;
}


ULONG DiffWindowTextArea::ScrollDown(ULONG numLines)
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

  // Move each text area downward by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 -numLines * m_FontHeight_pix,  // n * height
                 m_VScrollRect.Left(),
                 m_VScrollRect.Top(),
                 m_VScrollRect.Right(),
                 m_VScrollRect.Bottom());

  // Fill the gap with the previous text lines
  for(ULONG i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pDiffFile->GetLine(lineIndex);

    if(pLeftLine == NULL)
    {
      break;
    }

    drawDiffLine(pLeftLine, i * m_FontHeight_pix);
  }

  m_Y -= numLines;
  return numLines;
}


void DiffWindowTextArea::PrintPageAt(ULONG left, ULONG top)
{
  m_X = left;
  m_Y = top;

  PrintPage();
}


void DiffWindowTextArea::PrintPage()
{
  if(m_pDiffFile == NULL)
  {
    return;
  }

  for(ULONG i = m_Y; (i - m_Y) < m_MaxVisibleLines; i++)
  {
    if(i >= m_NumLines)
    {
      break;
    }

    const DiffLine* pLine = m_pDiffFile->GetLine(i);
    if(pLine == NULL)
    {
      break;
    }

    drawDiffLine(pLine, (i - m_Y) * m_FontHeight_pix);
  }
}


void DiffWindowTextArea::drawDiffLine(const DiffLine* pLine,
                                   WORD topEdge,
                                   bool bHorizontallyScrolled,
                                   int startIndex,
                                   int count)
{
  ULONG indent = 0;

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
    indent = (m_MaxVisibleChars - count) * m_FontWidth_pix;
  }

  if(!bHorizontallyScrolled && m_LineNumbersEnabled)
  {
    //
    // Print the line numbers in left and right
    //

    // Move rastport cursor to start of line numbers block
    Move(m_pRPorts->LineNum(),
        Left() + indent + 2,
        topEdge + Top() + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLine->LineNum();

    // Print line's original line number
    Text(m_pRPorts->LineNum(), pLineNum, m_LineNumsWidth_chars);
  }

  // Getting the RastPort for the line to draw in. This depends on
  // the line background color which itself depends on the diff state
  // of the line.
  RastPort* pRPort = diffStateToRastPort(pLine->State());

  long numCharsToPrint = calcNumPrintChars(pLine, count, startIndex);
  if(numCharsToPrint <= 0)
  {
    return;
  }

  // Move rastport cursor to start of line
  Move(pRPort,
       Left() + indent + m_LineNumsWidth_pix + 3,
       topEdge + Top() + m_FontBaseline_pix + 1);

  // Print line
  Text(pRPort,
        pLine->Txt() + startIndex,
        numCharsToPrint);
}


ULONG DiffWindowTextArea::calcNumPrintChars(const DiffLine* pDiffLine,
                                            int count,
                                            int startIndex)
{
  ULONG numCharsToPrint = 0;

  if(count > 0)
  {
    numCharsToPrint = count;
  }
  else
  {
    // Determine how many characters would be print theoretically
    numCharsToPrint = pDiffLine->NumChars() - m_X;
  }

  if(numCharsToPrint > m_MaxVisibleChars)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_MaxVisibleChars;
  }

  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pDiffLine->NumChars()))
  {
    // Limit the number of printed chars to line length
    numCharsToPrint = pDiffLine->NumChars() - startIndex;
  }

  return numCharsToPrint;
}


RastPort* DiffWindowTextArea::diffStateToRastPort(DiffLine::LineState state)
{
  if(state == DiffLine::Added)
  {
    return m_pRPorts->TextGreenBG();
  }
  else if(state== DiffLine::Changed)
  {
    return m_pRPorts->TextYellowBG();
  }
  else if(state == DiffLine::Deleted)
  {
    return m_pRPorts->TextRedBG();
  }
  else
  {
    return m_pRPorts->TextDefault();
  }
}
