#include <clib/graphics_protos.h>

#include <math.h>
#include <string.h>

#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(DiffWindowRastports*& pRPorts)
  : m_pRPorts(pRPorts),
    m_pDiffFile(NULL),
    m_LineNumbersWidth_pix(0),
    m_NumLines(0),
    m_MaxNumChars(0),
    m_MaxTextAreaChars(0),
    m_MaxTextAreaLines(0),
    m_X(0),
    m_Y(0),
    m_LineNumsWidth_chars(0),
    m_LineNumsWidth_pix(0),
    m_FontWidth_pix(0),
    m_FontHeight_pix(0),
    m_FontBaseline_pix(0)

{

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

ULONG DiffWindowTextArea::NumVisibleChars() const
{
  return m_MaxTextAreaChars;
}

ULONG DiffWindowTextArea::NumVisibleLines() const
{
  return m_MaxTextAreaLines;
}

void DiffWindowTextArea::SetDocument(const DiffOutputFileBase* pDiffFile, 
                                     struct TextFont* pTextFont,
                                     ULONG maxNumChars,
                                     bool lineNumbersEnabled)
{
  m_pDiffFile = pDiffFile;
  m_NumLines = pDiffFile->NumLines();
  m_MaxNumChars = maxNumChars;
  m_LineNumbersEnabled = lineNumbersEnabled;
  m_X = 0;
  m_Y = 0;

  m_FontWidth_pix = pTextFont->tf_XSize;
  m_FontHeight_pix = pTextFont->tf_YSize;
  m_FontBaseline_pix = pTextFont->tf_Baseline;

  if(lineNumbersEnabled)
  {
    const DiffLine* pLine = pDiffFile->GetLine(0);
    const char* pLineNum = pLine->LineNum();

    m_LineNumsWidth_chars = strlen(pLineNum);
    m_LineNumsWidth_pix = m_LineNumsWidth_chars * m_FontWidth_pix;
  }


  paintDocuments(true);
}


void DiffWindowTextArea::SetSizes(ULONG width, 
                                  ULONG height)
{
  m_MaxTextAreaLines = (height - 4) /  m_FontHeight_pix;;

  // Limit heigt to int multiples.
  height = m_MaxTextAreaLines * m_FontHeight_pix + 3;
 
  // Calculate how many chars fit on each line in each text area
  m_MaxTextAreaChars = (width - 7 - m_LineNumsWidth_pix) / m_FontWidth_pix;

  ULONG textAreasTextWidth_pix = m_MaxTextAreaChars * m_FontWidth_pix;
  textAreasTextWidth_pix += m_LineNumsWidth_pix;

  Rect::SetWidthHeight(width, height);

  //
  // Define two the scrolling areas. Note that there are two different 
  // scroll rects used because:
  //   The 'Left' values depend on if the scroll is done horizontally or
  //   vertically. On vertical scroll the block containing line numbers 
  //   is scrolled too. On horizontal scroll, it is not.
  //

  LONG maxTextWidth_pix = m_MaxTextAreaChars * m_FontWidth_pix;
  maxTextWidth_pix += m_LineNumsWidth_pix;

  m_HScrollRect.Set(Left() + m_LineNumbersWidth_pix + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);

  m_VScrollRect.Set(Left() + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);
}


void DiffWindowTextArea::ScrollTopToRow(size_t rowId)
{
  // Prevent to scroll below the last line
  ULONG yLimit = m_NumLines - m_MaxTextAreaLines;
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
  m_Y =  rowId;

  EraseRect(m_pRPorts->Window(),
            Left() + 2, 
            Top() + 2,
            Right() - 3,
            Bottom() - 3);

  paintDocuments(false);
}


void DiffWindowTextArea::ScrollLeftToColumn(size_t columId)
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
  m_X = columId;

  // Clear ext area completely
  Clear();

  paintDocuments(false);
}


void DiffWindowTextArea::Clear()
{
  // EraseRect(m_pRPorts->LineNum(),
  //           Left() + 2, 
  //           Top() + 2,
  //           Right() - 3,
  //           Bottom() - 3);
}

const Rect& DiffWindowTextArea::HScroll()
{
  return m_HScrollRect;
}

const Rect& DiffWindowTextArea::VScroll()
{
  return m_VScrollRect;
}



size_t DiffWindowTextArea::scrollRight(size_t numChars)
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


  // Move each text area right by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 -numChars * m_FontWidth_pix, // n * width
                 0,
                 m_HScrollRect.Left(),
                 m_HScrollRect.Top(),
                 m_HScrollRect.Right(),
                 m_HScrollRect.Bottom());


  // fill the gap with the previous chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLine = m_pDiffFile->GetLine(i);
    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine,
              (i - m_Y) * m_FontHeight_pix,
              true,
              m_X - numChars,
              numChars);

  }

  return numChars;
}


size_t DiffWindowTextArea::scrollLeft(size_t numChars)
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

  if(m_MaxNumChars < m_MaxTextAreaChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxTextAreaChars) == m_MaxNumChars)
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_MaxTextAreaChars + numChars) > m_MaxNumChars)
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_MaxNumChars - (m_X + m_MaxTextAreaChars);
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
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLine = m_pDiffFile->GetLine(i);

    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine,
              (i - m_Y)  * m_FontHeight_pix,
              true,
              m_X + m_MaxTextAreaChars,
              -numChars);

  }

  return numChars;
}


size_t DiffWindowTextArea::scrollDown(size_t numLines)
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
  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pDiffFile->GetLine(lineIndex);

    if(pLeftLine == NULL)
    {
      break;
    }

    paintLine(pLeftLine, i * m_FontHeight_pix);
  }

  return numLines;
}


size_t DiffWindowTextArea::scrollUp(size_t numLines)
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

  // Move each text area upward by n * the height of one text line
  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 numLines * m_FontHeight_pix,
                 m_VScrollRect.Left(),
                 m_VScrollRect.Top(),
                 m_VScrollRect.Right(),
                 m_VScrollRect.Bottom());

  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y + m_MaxTextAreaLines + i;
    const DiffLine* pLine = m_pDiffFile->GetLine(lineIndex);
    if(pLine == NULL)
    {
      break;
    }

    int paintLineIndex = m_MaxTextAreaLines - numLines + i;
    paintLine(pLine,
              paintLineIndex * m_FontHeight_pix);
  }

  return numLines;
}



void DiffWindowTextArea::paintDocuments(bool bFromStart)
{
  if(m_pDiffFile == NULL)
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

    const DiffLine* pLine = m_pDiffFile->GetLine(i);
    if(pLine == NULL)
    {
      break;
    }

    paintLine(pLine, (i - m_Y) * m_FontHeight_pix);
  }
}


void DiffWindowTextArea::paintLine(const DiffLine* pLine,
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

  if(!bHorizontallyScrolled && m_LineNumbersEnabled)
  {
    //
    // Print the line numbers in left and right
    //

    // Move rastport cursor to start of line numbers block
    ::Move(m_pRPorts->LineNum(),
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
  ::Move(pRPort,
        Left() + indent + m_LineNumsWidth_pix + 3,
        topEdge + Top() + m_FontBaseline_pix + 1);

  // Print line
  Text(pRPort,
        pLine->Text() + startIndex,
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
