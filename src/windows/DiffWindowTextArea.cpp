#ifdef __clang__
  #include <clib/graphics_protos.h>
#else
  #include <proto/graphics.h>
#endif

#include <math.h>
#include <string.h>

#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(const DiffOutputFileBase& diffFile,
                                       DiffWindowRastports*& pRPorts,
                                       struct TextFont* pTextFont,
                                       bool lineNumbersEnabled,
                                       ULONG maxNumChars)
  : m_DiffFile(diffFile),
    m_pRPorts(pRPorts),
    m_LineNumbersEnabled(lineNumbersEnabled),
    m_MaxNumChars(maxNumChars),
    m_NumLines(diffFile.getNumLines()),
    m_FontWidth_pix(pTextFont->tf_XSize),
    m_FontHeight_pix(pTextFont->tf_YSize),
    m_FontBaseline_pix(pTextFont->tf_Baseline),
    m_AreaCharWidth(0),
    m_AreaLineHeight(0),
    m_X(0),
    m_Y(0),
    m_LineNumsWidth_chars(0),
    m_LineNumsWidth_pix(0)
{
  if(lineNumbersEnabled)
  {
    const DiffLine* pLine = diffFile[0];
    const char* pLineNum = pLine->getLineNumText();

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
  return m_AreaCharWidth;
}


ULONG DiffWindowTextArea::MaxVisibleLines() const
{
  return m_AreaLineHeight;
}

#include <stdio.h>

void DiffWindowTextArea::SetWidthHeight(unsigned long width,
                                        unsigned long height)
{

  m_AreaLineHeight = (height - 4) /  m_FontHeight_pix;
  m_AreaCharWidth = (width - 7 - m_LineNumsWidth_pix) / m_FontWidth_pix;

  // Limit height to int multiples
  height = m_AreaLineHeight * m_FontHeight_pix + 3;

  printf("Set width %lu -> %lu, height %lu -> %lu\n", Width(), width, Height(), height);
  Rect::SetWidthHeight(width, height);

  //
  // Define two the scrolling areas. Note that there are two different
  // scroll rects used because:
  //   The 'Left' values depend on if the scroll is done horizontally or
  //   vertically. On vertical scroll the block containing line numbers
  //   is scrolled too. On horizontal scroll, it is not.
  //

  ULONG maxTextWidth_pix = m_AreaCharWidth * m_FontWidth_pix;
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
  // EraseRect(m_pRPorts->getLineNumText(),
  //           Left() + 2,
  //           Top() + 2,
  //           Right() - 3,
  //           Bottom() - 3);
}


void DiffWindowTextArea::AddSelectedText(ULONG lineId, 
                                         ULONG fromColumn, 
                                         ULONG toColumn)
{
  m_DiffFile.addSelection(lineId, fromColumn, toColumn);
}


void DiffWindowTextArea::ScrollTopToRow(ULONG rowId)
{
  // Prevent to scroll below the last line
  ULONG yLimit = m_NumLines - m_AreaLineHeight;
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

  int deltaLimit = m_AreaLineHeight - 2;
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

  if(numChars > m_AreaCharWidth)
  {
    numChars = m_AreaCharWidth;
  }

  if(m_MaxNumChars < m_AreaCharWidth)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_AreaCharWidth) == m_MaxNumChars)
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_AreaCharWidth + numChars) > m_MaxNumChars)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_MaxNumChars - (m_X + m_AreaCharWidth);
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
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaLineHeight; lineId++)
  {
    printDiffLine(lineId,
                  (lineId - m_Y)  * m_FontHeight_pix,
                  true,
                  m_X + m_AreaCharWidth,
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

  if(numChars > m_AreaCharWidth)
  {
    numChars = m_AreaCharWidth;
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
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaLineHeight; lineId++)
  {
    printDiffLine(lineId,
                  (lineId - m_Y) * m_FontHeight_pix,
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

  if(m_NumLines < m_AreaLineHeight)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return 0;
  }

  if((m_Y + m_AreaLineHeight) == m_NumLines)
  {
    // Do not move the scroll area upward if text already at bottom
    return 0;
  }

  if((m_Y + m_AreaLineHeight + numLines) > m_NumLines)
  {
    // Limit the scrolling to only scroll only as many lines as necessary
    numLines = m_NumLines - (m_Y + m_AreaLineHeight);
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
    ULONG lineId = m_Y + m_AreaLineHeight + i;
    WORD topEdge = m_AreaLineHeight - numLines + i;
    printDiffLine(lineId, topEdge * m_FontHeight_pix);
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
    int lineId = m_Y - numLines + i;
    printDiffLine(lineId, i * m_FontHeight_pix);
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
  for(ULONG lineId = m_Y; (lineId - m_Y) < m_AreaLineHeight; lineId++)
  {
    if(lineId >= m_NumLines)
    {
      break;
    }

    printDiffLine(lineId, (lineId - m_Y) * m_FontHeight_pix);
  }
}


void DiffWindowTextArea::printDiffLine(ULONG lineId,
                                       WORD topEdge,
                                       bool bHorizontallyScrolled,
                                       int startIndex,
                                       int numChars)
{
  const DiffLine* pLine = m_DiffFile[lineId];
  if(pLine == NULL)
  {
    printf("RETURNING because 'pLine == NULL'\n");
    return;
  }


  if(startIndex < 0)
  {
    startIndex = m_X;
  }

  ULONG indent = 0;
  if(numChars < 0)
  {
    // A negative number means that only the given number of chars is 
    // inserted, counted from the right. So an indent for the text is
    // calculated and numChars is made positive to get used below.
    numChars = -numChars;
    indent = (m_AreaCharWidth - numChars) * m_FontWidth_pix;
  }

  if(!bHorizontallyScrolled && m_LineNumbersEnabled)
  {
    //
    // Print the line numbers
    //

    // Move rastport cursor to start of line numbers block
    Move(m_pRPorts->getLineNumText(),
        Left() + indent + 2,
        topEdge + Top() + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLine->getLineNumText();

    // Print line's original line number
    Text(m_pRPorts->getLineNumText(), pLineNum, m_LineNumsWidth_chars);
  }

  RastPort* pRPort;
  long currentColumn = 0;
  long n = 0;

  do
  {
    if((n = m_DiffFile.getNumNormalChars(lineId, currentColumn)) > 0)
    {
      // Get the RastPort for the line to draw. Depends on the diff state
      // of the line.
      // printf("SET RPORT to 'DIFF COLOR' for line %d ", lineId);
      pRPort = diffStateToRastPort(pLine->getState());
    }
    else if ((n = m_DiffFile.getNumMarkedChars(lineId, currentColumn)) > 0)
    {
      // printf("SET RPORT to 'SELECTED TEXT' for line %d ", lineId);
      pRPort = m_pRPorts->TextSelected();
    }
    else
    {
      // printf("RETURNING from line %d because no marked / normal chars anymore.\n");
      return;
    }

    // printf(" and PRINT %d chars.\n", n);

    // Move rastport cursor to start of line
    Move(pRPort,
        Left() + indent + m_FontWidth_pix * currentColumn + m_LineNumsWidth_pix + 3,
        topEdge + Top() + m_FontBaseline_pix + 1);

    // Print line
    Text(pRPort,
         pLine->getText() + startIndex + currentColumn,
         n);

    currentColumn += n;
  } 
  while (n > 0); 

  // long numCharsToPrint = calcNumPrintChars(pLine, numChars, startIndex);
  // if(numCharsToPrint <= 0)
  // {
  //   return;
  // }


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
    numCharsToPrint = pDiffLine->getNumChars() - m_X;
  }

  if(numCharsToPrint > m_AreaCharWidth)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_AreaCharWidth;
  }

  if((startIndex > -1) &&
     (numCharsToPrint + startIndex > pDiffLine->getNumChars()))
  {
    // Limit the number of printed chars to line length
    numCharsToPrint = pDiffLine->getNumChars() - startIndex;
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
