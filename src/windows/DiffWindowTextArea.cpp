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
                                       TextFont* pTextFont,
                                       bool lineNumbersEnabled,
                                       ULONG maxNumChars)
  : m_DiffFile(diffFile),
    m_pRPorts(pRPorts),
    m_AreLineNumbersEnabled(lineNumbersEnabled),
    m_LongestLineChars(maxNumChars),
    m_TabWidth(8),  // TODO Parametrize
    m_pLineOfSpaces(NULL),
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
  if(m_pLineOfSpaces != NULL)
  {
    delete[] m_pLineOfSpaces;
    m_pLineOfSpaces = NULL;
  }
}


ULONG DiffWindowTextArea::getX() const
{
  return m_X;
}


ULONG DiffWindowTextArea::getY() const
{
  return m_Y;
}


bool DiffWindowTextArea::isScrolledToTop() const
{
  return m_Y < 1;
}

bool DiffWindowTextArea::isScrolledToBottom() const
{
  return (m_Y + m_AreaMaxLines) >= m_DiffFile.getNumLines();
}

bool DiffWindowTextArea::isLineVisible(ULONG lineId) const
{
  return (lineId >= m_Y) && (lineId < (m_Y + m_AreaMaxLines));
}


ULONG DiffWindowTextArea::getMaxVisibleChars() const
{
  return m_AreaMaxChars;
}


ULONG DiffWindowTextArea::getMaxVisibleLines() const
{
  return m_AreaMaxLines;
}


void DiffWindowTextArea::setSize(ULONG width, ULONG height)
{
  m_AreaMaxLines = (height - 4) /  m_FontHeight_pix;
  m_AreaMaxChars = (width - 7 - m_LineNumsWidth_pix) / m_FontWidth_pix;

  // Create a full line of spaces, to be used for tab rendering
  if(m_pLineOfSpaces != NULL)
  {
    delete[] m_pLineOfSpaces;
    m_pLineOfSpaces = NULL;
  }

  m_pLineOfSpaces = new char[m_AreaMaxChars + 1];
  memset(m_pLineOfSpaces, ' ', m_AreaMaxChars);
  m_pLineOfSpaces[m_AreaMaxChars] = '\0';

  // Limit height to whole lines
  height = m_AreaMaxLines * m_FontHeight_pix + 3;

  // Set the outer rect of this text area to new width and height
  Rect::setSize(width, height);

  /**
   * NOTE: The TextArea 'is' a Rect by itself but it additionally
   * contains two other rects which are needed for scrolling. The reason
   * two other rects are needed is:
   *
   *   The 'Left edge' values of the scrolling area depend on if the
   *   scroll is done horizontally or vertically. On vertical scroll the
   *   block containing line numbers is scrolled too. On horizontal
   *   scroll, it is not.
   */

  ULONG maxTextWidth_pix = m_AreaMaxChars * m_FontWidth_pix;
  maxTextWidth_pix += m_LineNumsWidth_pix;

  m_HScrollRect.set(getLeft() + m_LineNumsWidth_pix + 2,
                    getTop() + 1,
                    getLeft() + maxTextWidth_pix + 2,
                    getTop() + getHeight() - 3);

  m_VScrollRect.set(getLeft() + 2,
                    getTop() + 1,
                    getLeft() + maxTextWidth_pix + 2,
                    getTop() + getHeight() - 3);
}


void DiffWindowTextArea::addSelection(ULONG lineId, 
                                      ULONG fromColumn, 
                                      ULONG toColumn)
{
  m_DiffFile.addSelection(lineId, fromColumn, toColumn);
}


void DiffWindowTextArea::clearSelection()
{
  m_DiffFile.clearSelection();
}


void DiffWindowTextArea::scrollTopToRow(ULONG rowId)
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
      scrollUp(deltaAbs);
    }
    else if(delta < 0)
    {
      scrollDown(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // y-position
  //
  m_Y =  rowId;

  EraseRect(m_pRPorts->Window(),
            m_VScrollRect.getLeft(),
            m_VScrollRect.getTop(),
            m_VScrollRect.getRight(),
            m_VScrollRect.getBottom());

  printPage();
}


void DiffWindowTextArea::scrollLeftToColumn(ULONG columId)
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
      scrollLeft(deltaAbs);
    }
    else if(delta < 0)
    {
      scrollRight(deltaAbs);
    }

    return;
  }

  //
  // Scroll bigger amounts by re-painting the whole page at the new
  // x-position
  //
  m_X = columId;

  // Clear the area except the line numbers area
  EraseRect(m_pRPorts->Window(),
            m_HScrollRect.getLeft(),
            m_HScrollRect.getTop(),
            m_HScrollRect.getRight(),
            m_HScrollRect.getBottom());

  printPage(true);
}


ULONG DiffWindowTextArea::scrollLeft(ULONG numChars)
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
                 m_HScrollRect.getLeft(),
                 m_HScrollRect.getTop(),
                 m_HScrollRect.getRight(),
                 m_HScrollRect.getBottom());

  // Fill the gap with the following chars
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    renderLine(lineId, false, lineTopEdge, -numChars);
  }

  m_X += numChars;
  return numChars;
}


ULONG DiffWindowTextArea::scrollRight(ULONG numChars)
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
                 m_HScrollRect.getLeft(),
                 m_HScrollRect.getTop(),
                 m_HScrollRect.getRight(),
                 m_HScrollRect.getBottom());


  // fill the gap with the previous chars
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    renderLine(lineId, false, lineTopEdge, numChars);
  }

  m_X -= numChars;
  return numChars;
}


ULONG DiffWindowTextArea::scrollUp(ULONG numLines)
{
  if(numLines < 1)
  {
    // Noting to do
    return 0;
  }

  if(m_DiffFile.getNumLines() <= m_AreaMaxLines)
  {
    // All text fits into the window. Don't scroll.
    return 0;
  }

  if(numLines > m_AreaMaxLines)
  {
    numLines = m_AreaMaxLines;
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
                 m_VScrollRect.getLeft(),
                 m_VScrollRect.getTop(),
                 m_VScrollRect.getRight(),
                 m_VScrollRect.getBottom());

  // Fill the now empty lines at the bottom with the next lines 
  for(ULONG i = 0; i < numLines; i++)
  {
    ULONG lineId = m_Y + m_AreaMaxLines + i;
    WORD lineTopEdge = (m_AreaMaxLines - numLines + i) * m_FontHeight_pix;
    renderLine(lineId, true, lineTopEdge);
  }

  m_Y += numLines;

  return numLines;
}


ULONG DiffWindowTextArea::scrollDown(ULONG numLines)
{
  if(numLines < 1 || m_Y < 1)
  {
    // Nothing to do
    return 0;
  }

  if(numLines > m_Y)
  {
    // Limit to not exceed / go below lineId 0
    numLines = m_Y;
  }

  if(numLines > m_AreaMaxLines)
  {
    // Limit to scroll maximum one page.
    // TODO Why is this needed here?
    numLines = m_AreaMaxLines;
  }

  // Move each text area downward by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 -numLines * m_FontHeight_pix,  // n * height
                 m_VScrollRect.getLeft(),
                 m_VScrollRect.getTop(),
                 m_VScrollRect.getRight(),
                 m_VScrollRect.getBottom());

  // Fill the now empty lines at the top with the previous text lines
  for(ULONG i = 0; i < numLines; i++)
  {
    ULONG lineId = m_Y - numLines + i;
    WORD lineTopEdge = i * m_FontHeight_pix;
    renderLine(lineId, true, lineTopEdge);
  }
  
  m_Y -= numLines;

  return numLines;
}


void DiffWindowTextArea::printPageAt(ULONG left, ULONG top)
{
  m_X = left;
  m_Y = top;

  printPage();
}


void DiffWindowTextArea::printPage(bool dontPrintLineNumbers)
{
  for(ULONG lineId = m_Y; lineId < m_Y + m_AreaMaxLines; lineId++)
  {
    WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
    renderLine(lineId, !dontPrintLineNumbers, lineTopEdge);
  }
}

void DiffWindowTextArea::printLine(ULONG lineId)
{
  WORD lineTopEdge = (lineId - m_Y) * m_FontHeight_pix;
  renderLine(lineId, true, lineTopEdge);
}


void DiffWindowTextArea::renderLine(ULONG lineId, 
                                    bool doDisplayLineNumbers, 
                                    long lineTop, 
                                    long numCharLimit)
{
  const DiffLine* pLine = m_DiffFile[lineId];
  if(pLine == NULL)
  {
    return;
  }

  /**
   * Render the line numbers
   */

  if(doDisplayLineNumbers && m_AreLineNumbersEnabled)
  {
    // Move rastport cursor to start of line numbers block
    Move(m_pRPorts->getLineNumText(),
         m_VScrollRect.getLeft(),
         getTop() + lineTop + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLine->getLineNumText();

    // Print line's original line number
    Text(m_pRPorts->getLineNumText(), pLineNum, m_LineNumsWidth_chars);
  }


  /**
   * Render the line
   */

  ULONG currentTextColumn;
  ULONG currentDisplayColumn;
  long numRemainingChars; // Number of ramaining chars for rendering on current line
  if(numCharLimit < 0)
  {
    // Only render 'numCharLimit' chars  at the right of the lines
    // visible area
    numRemainingChars = -numCharLimit;

    currentTextColumn = m_AreaMaxChars + m_X;
    currentDisplayColumn = m_AreaMaxChars - numRemainingChars;
  }
  else if(numCharLimit > 0)
  {
    // Only render 'numCharLimit' chars at the left of the lines visible
    // area
    numRemainingChars = numCharLimit;
    currentTextColumn = m_X - numRemainingChars;
    currentDisplayColumn = 0;
  }
  else
  {
    // Default, no scrolling
    numRemainingChars = m_AreaMaxChars;
    currentTextColumn = m_X;
    currentDisplayColumn = 0;
  }

  // Get the text position info of resulting text column. This also
  // calculates the srcTextColumn which is needed next.
  ULONG resultingTextColumn = currentTextColumn;
  TextPositionInfo positionInfo;
  positionInfo = getTextPositionInfo(pLine->getText(),
                                      pLine->getNumChars(),
                                      resultingTextColumn);

  // Get the RastPort to render the next block of marked / not marked
  // text in the line.
  ULONG numCharsInBlock;
  RastPort* pRPort;
  if((numCharsInBlock = m_DiffFile.getNumNormalChars(lineId, positionInfo.srcTextColumn)) > 0)
  {
    // The RastPort of the normal, not marked text depends on the diff
    // state of the line.
    pRPort = diffStateToRastPort(pLine->getState());
  }
  else if ((numCharsInBlock = m_DiffFile.getNumMarkedChars(lineId, positionInfo.srcTextColumn)) > 0)
  {
    pRPort = m_pRPorts->TextSelected();
  }
  else
  {
    return;
  }

  ULONG nextNumCharsToPrint;
  const char* pTextToPrint;
  bool hasMarkedNormalBlockLimitReached = false;
  bool hasNumCharsBeenLimited = false;

  while(numRemainingChars > 0 &&
        (positionInfo.numRemainingChars > 0 || positionInfo.numRemainingSpaces > 0))
  {
    if(positionInfo.numRemainingChars > 0)
    {
      // Set the text print pointer to te nax char to be printed
      nextNumCharsToPrint = positionInfo.numRemainingChars;
      pTextToPrint = pLine->getText() + positionInfo.srcTextColumn;
    }
    else
    {
      // Set the text print pointer to the line of spaces
      nextNumCharsToPrint = positionInfo.numRemainingSpaces;
      pTextToPrint = m_pLineOfSpaces;
    }

    if(nextNumCharsToPrint > numCharsInBlock)
    {
      nextNumCharsToPrint = numCharsInBlock;
      hasMarkedNormalBlockLimitReached = true;
    }

    if(nextNumCharsToPrint > numRemainingChars)
    {
      nextNumCharsToPrint = numRemainingChars;
      hasNumCharsBeenLimited = true;
    }
    
    // Limit the number of Chars to print to the maximum
    if(currentDisplayColumn + nextNumCharsToPrint > m_AreaMaxChars)
    {
      nextNumCharsToPrint = m_AreaMaxChars - currentDisplayColumn;
      hasNumCharsBeenLimited = true;
    }

    // Move rastport cursor to start of rendering
    Move(pRPort,
        m_HScrollRect.getLeft() + m_FontWidth_pix * currentDisplayColumn,
        getTop() + lineTop + m_FontBaseline_pix + 1);

    // Render the text
    Text(pRPort,
        pTextToPrint,
        nextNumCharsToPrint);

    if(hasMarkedNormalBlockLimitReached)
    {
      break;
    }

    if(hasNumCharsBeenLimited)
    {
      // Nothing more to print as number of chars to print had already
      // been limited
      return;
    }

    if(positionInfo.numRemainingChars > 0)
    {
      currentTextColumn += nextNumCharsToPrint;
    }
    else
    {
      currentTextColumn++;
    }

    numRemainingChars -= nextNumCharsToPrint;
    currentDisplayColumn += nextNumCharsToPrint;
    resultingTextColumn += nextNumCharsToPrint;

    positionInfo = getTextPositionInfo(pLine->getText(),
                                        pLine->getNumChars(),
                                        resultingTextColumn);
  }
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






TextPositionInfo DiffWindowTextArea::getTextPositionInfo(const char* pSrcText, 
                                                         ULONG srcTextLength, 
                                                         ULONG resultingTextColumn)
{
  TextPositionInfo info = {0};
  ULONG i, accumulatedColumn, tabIndent;

  accumulatedColumn = 0;

  // Parse each character of input text
  for(info.srcTextColumn = 0; info.srcTextColumn < srcTextLength; info.srcTextColumn++)
  {
    if(accumulatedColumn >= resultingTextColumn)
    {
      tabIndent = m_TabWidth - (size_t)(resultingTextColumn % m_TabWidth);

      if(accumulatedColumn > resultingTextColumn)
      {
        // In midst of / among a tabulator block
        info.numRemainingChars = 0;
        info.numRemainingSpaces = tabIndent;
      }
      else
      {
        if(((info.srcTextColumn > 1) && (pSrcText[info.srcTextColumn] == '\t')) || 
           ((info.srcTextColumn == 0) && (pSrcText[info.srcTextColumn] == '\t')))
        {
          // Directly on the start of a tabulator block
          info.numRemainingChars = 0;
          info.numRemainingSpaces = tabIndent;
        }
        else
        {
          // A printable character, no tabulator block
          
          // Check how many chars / spaces until next tab position or eol
          for(i = info.srcTextColumn; i < srcTextLength; i++)
          {
            if(pSrcText[i] == '\t')
            {
              break;
            }
          }

          info.numRemainingChars = i - info.srcTextColumn;
          info.numRemainingSpaces = 0;
        }
      }

      return info;
    }

    if(pSrcText[info.srcTextColumn] == '\t')
    {
      // Increase actual result column by current position tabulator indent
      accumulatedColumn += (size_t)( m_TabWidth - (accumulatedColumn % m_TabWidth));
    }
    else
    {
      // Increase actual result column by one
      accumulatedColumn++;
    }
  }

  return info;
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
