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
    m_AreLineNumbersEnabled(lineNumbersEnabled),
    m_LongestLineChars(maxNumChars),
    m_FontWidth_pix(pTextFont->tf_XSize),
    m_FontHeight_pix(pTextFont->tf_YSize),
    m_FontBaseline_pix(pTextFont->tf_Baseline),
    m_AreaMaxChars(0),
    m_AreaMaxLines(0),
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
  return m_AreaMaxChars;
}


ULONG DiffWindowTextArea::MaxVisibleLines() const
{
  return m_AreaMaxLines;
}


void DiffWindowTextArea::SetSize(ULONG width, ULONG height)
{
  m_AreaMaxLines = (height - 4) /  m_FontHeight_pix;
  m_AreaMaxChars = (width - 7 - m_LineNumsWidth_pix) / m_FontWidth_pix;

  // Limit height to int multiples
  height = m_AreaMaxLines * m_FontHeight_pix + 3;

  Rect::SetSize(width, height);

  //
  // Define two the scrolling areas. Note that there are two different
  // scroll rects used because:
  //   The 'Left' values depend on if the scroll is done horizontally or
  //   vertically. On vertical scroll the block containing line numbers
  //   is scrolled too. On horizontal scroll, it is not.
  //

  ULONG maxTextWidth_pix = m_AreaMaxChars * m_FontWidth_pix;
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
  ULONG yLimit = m_DiffFile.getNumLines() - m_AreaMaxLines;
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

  int deltaLimit = m_AreaMaxLines - 2;
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

  if(numChars > m_AreaMaxChars)
  {
    numChars = m_AreaMaxChars;
  }

  if(m_LongestLineChars < m_AreaMaxChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_AreaMaxChars) == m_LongestLineChars)
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_AreaMaxChars + numChars) > m_LongestLineChars)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_LongestLineChars - (m_X + m_AreaMaxChars);
  }

  // Move text area content left by n * the width of one char
  ScrollRasterBF(m_pRPorts->Window(),
                 numChars * m_FontWidth_pix,
                 0,
                 m_HScrollRect.Left(),
                 m_HScrollRect.Top(),
                 m_HScrollRect.Right(),
                 m_HScrollRect.Bottom());

  // Fill the gap with the following chars
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    printDiffLine(lineId, false, lineTopEdge, -numChars);
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

  if(numChars > m_AreaMaxChars)
  {
    numChars = m_AreaMaxChars;
  }


  // Move text area content right by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 -numChars * m_FontWidth_pix, // n * width
                 0,
                 m_HScrollRect.Left(),
                 m_HScrollRect.Top(),
                 m_HScrollRect.Right(),
                 m_HScrollRect.Bottom());


  // fill the gap with the previous chars
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    printDiffLine(lineId, false, lineTopEdge, numChars);
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

  if(m_DiffFile.getNumLines() <= m_AreaMaxLines)
  {
    // All text fits into the window. Dont't scroll.
    return 0;
  }

  if((m_Y + m_AreaMaxLines + numLines) > m_DiffFile.getNumLines())
  {
    // Limit the scrolling to not exceed number of lines in file
    numLines = m_DiffFile.getNumLines() - m_Y - m_AreaMaxLines;
  }

  // Move each text area upward by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 numLines * m_FontHeight_pix,
                 m_VScrollRect.Left(),
                 m_VScrollRect.Top(),
                 m_VScrollRect.Right(),
                 m_VScrollRect.Bottom());

  // Fill the now empty lines at the bottom with the next lines 
  for(ULONG i = 0; i < numLines; i++)
  {
    ULONG lineId = m_Y + m_AreaMaxLines + i;
    WORD lineTopEdge = (m_AreaMaxLines - numLines + i) * m_FontHeight_pix;
    printDiffLine(lineId, true, lineTopEdge);
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

  // Fill the now empty lines at the top with the previous text lines
  for(ULONG i = 0; i < numLines; i++)
  {
    ULONG lineId = m_Y - numLines + i;
    WORD lineTopEdge = i * m_FontHeight_pix;
    printDiffLine(lineId, true, lineTopEdge);
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
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    printDiffLine(lineId, true, lineTopEdge);
  }
}


void DiffWindowTextArea::printDiffLine(ULONG lineId, 
                                       bool doDisplayLineNumbers, 
                                       long lineTop, 
                                       long numCharLimit)
{
  const DiffLine* pLine = m_DiffFile[lineId];
  if(pLine == NULL)
  {
    return;
  }

  //
  // Print the line numbers
  //
  if(doDisplayLineNumbers && m_AreLineNumbersEnabled)
  {
    // Move rastport cursor to start of line numbers block
    Move(m_pRPorts->getLineNumText(),
         m_VScrollRect.Left(),
         Top() + lineTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLine->getLineNumText();

    // Print line's original line number
    Text(m_pRPorts->getLineNumText(), pLineNum, m_LineNumsWidth_chars);
  }



  ULONG currentTextColumn;
  ULONG currentDisplayColumn;

  if(numCharLimit < 0)
  {
    // Only display the right 'onlyNumChars' chars of the line's visible text
    numCharLimit = -numCharLimit;

    currentTextColumn = m_AreaMaxChars + m_X;
    currentDisplayColumn = m_AreaMaxChars - numCharLimit;
  }
  else if(numCharLimit > 0)
  {
    // Only display the left 'onlyNumChars' chars of the line's visible text
    currentTextColumn = m_X - numCharLimit;
    currentDisplayColumn = 0;
  }
  else
  {
    // Default, no scrolling
    currentTextColumn = m_X;
    currentDisplayColumn = 0;
  }

  long numCharsToPrint = 0;

  do
  {
    long sumPrintedChars = 0;
    bool hasNumCharsBeenLimited = false;
    RastPort* pRPort;

    if((numCharsToPrint = m_DiffFile.getNumNormalChars(lineId, currentTextColumn)) > 0)
    {
      // Get the RastPort for the line to draw. Depends on the diff state
      // of the line.
      pRPort = diffStateToRastPort(pLine->getState());
    }
    else if ((numCharsToPrint = m_DiffFile.getNumMarkedChars(lineId, currentTextColumn)) > 0)
    {
      pRPort = m_pRPorts->TextSelected();
    }
    else
    {
      return;
    }

    // Move rastport cursor to start of line
    Move(pRPort,
         m_HScrollRect.Left() + m_FontWidth_pix * currentDisplayColumn,
         Top() + lineTop + m_FontBaseline_pix + 1);

    // Print line
    if(currentDisplayColumn + numCharsToPrint > m_AreaMaxChars)
    {
      numCharsToPrint = m_AreaMaxChars - currentDisplayColumn;
      hasNumCharsBeenLimited = true;
    }

    if(numCharLimit > 0)
    {
      if(sumPrintedChars + numCharsToPrint > numCharLimit)
      {
        numCharsToPrint = numCharLimit - sumPrintedChars;
        hasNumCharsBeenLimited = true;
      }
    }

    Text(pRPort,
         pLine->getText() + currentTextColumn,
         numCharsToPrint);

    if(hasNumCharsBeenLimited)
    {
      // Nothing more to print as number of chars to print had already
      // been limited
      return;
    }

    currentTextColumn += numCharsToPrint;
    currentDisplayColumn += numCharsToPrint;
    sumPrintedChars += numCharsToPrint;
  } 
  while (numCharsToPrint > 0); 
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

  if(numCharsToPrint > m_AreaMaxChars)
  {
    // Limit the number of printed chars to fit into the text area
    numCharsToPrint = m_AreaMaxChars;
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
