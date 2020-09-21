#include <clib/graphics_protos.h>

#include <math.h>

#include "DiffWindowTextArea.h"

DiffWindowTextArea::DiffWindowTextArea(DiffWindowRastports*& pRPorts, 
                                       const UWORD& lineNumbersWidth_pix)
  : m_pRPorts(pRPorts),
    m_pDiffFile(NULL),
    m_LineNumbersWidth_pix(lineNumbersWidth_pix),
    m_NumLines(0),
    m_MaxTextAreaChars(0),
    m_MaxTextAreaLines(0),
    m_X(0),
    m_Y(0)

{

}

DiffWindowTextArea::~DiffWindowTextArea()
{

}


void DiffWindowTextArea::SetDocument(const DiffOutputFileBase* pDiffFile)
{
  m_pDiffFile = pDiffFile;
  m_NumLines = pDiffFile->NumLines();
  m_X = 0;
  m_Y = 0;

  paintDocuments(true);
}


void DiffWindowTextArea::ScrollToTopRow(size_t rowId)
{
  // Prevent to scroll below the last line
  long yLimit = m_NumLines - m_MaxTextAreaLines;
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


void DiffWindowTextArea::ScrollToLeftColumn(size_t columId)
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

void DiffWindowTextArea::SetWidthHeightScroll(long width, 
                                              long height, 
                                              long maxTextWidth_pix)
{
  Rect::SetWidthHeight(width, height);

  //
  // Define the dimensions for the scroll areas
  // NOTE: The 'Left' values depend on if the scroll is done 
  //       horizontally or vertically. On vertical scroll the line 
  //       numbers are scrolled too. On horizontal scroll, they're not.
  //
  m_HScrollRect.Set(Left() + m_LineNumbersWidth_pix + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);

  m_VScrollRect.Set(Left() + 3,
                    Top() + 1,
                    Left() + maxTextWidth_pix + 2,
                    Top() + Height() - 3);
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
                 m_TextArea1.HScroll().Left(),
                 m_TextArea1.HScroll().Top(),
                 m_TextArea1.HScroll().Right(),
                 m_TextArea1.HScroll().Bottom());

  ScrollRasterBF(m_pRPorts->Window(),
                 -numChars * m_FontWidth_pix,  // n * width
                 0,
                 m_TextArea2.HScroll().Left(),
                 m_TextArea2.HScroll().Top() + 1,
                 m_TextArea2.HScroll().Right(),
                 m_TextArea2.HScroll().Bottom());

  // fill the gap with the previous chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

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

  if(m_pDocument->MaxLineLength() < m_MaxTextAreaChars)
  {
    // Do not move the scroll area left if all the text fits into
    // the window
    return 0;
  }

  if((m_X + m_MaxTextAreaChars) == m_pDocument->MaxLineLength())
  {
    // Do not move the scroll area left if text already at rightmost
    // position
    return 0;
  }

  if((m_X + m_MaxTextAreaChars + numChars) > m_pDocument->MaxLineLength())
  {
    // Limit the scrolling to only scroll only as many chars as necessary
    numChars = m_pDocument->MaxLineLength() - (m_X + m_MaxTextAreaChars);
  }

  // Move each text area left by n * the width of one char
  ScrollRasterBF(m_pRPorts->Window(),
                 numChars * m_FontWidth_pix,
                 0,
                 m_TextArea1.HScroll().Left(),
                 m_TextArea1.HScroll().Top(),
                 m_TextArea1.HScroll().Right(),
                 m_TextArea1.HScroll().Bottom());

  ScrollRasterBF(m_pRPorts->Window(),
                 numChars * m_FontWidth_pix,
                 0,
                 m_TextArea2.HScroll().Left(),
                 m_TextArea2.HScroll().Top(),
                 m_TextArea2.HScroll().Right(),
                 m_TextArea2.HScroll().Bottom());

  // Fill the gap with the following chars
  for(unsigned long i = m_Y; i < m_Y + m_MaxTextAreaLines; i++)
  {
    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

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
                 m_TextArea1.VScroll().Left(),
                 m_TextArea1.VScroll().Top(),
                 m_TextArea1.VScroll().Right(),
                 m_TextArea1.VScroll().Bottom());

  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 -numLines * m_FontHeight_pix,  // n * height
                 m_TextArea2.VScroll().Left(),
                 m_TextArea2.VScroll().Top(),
                 m_TextArea2.VScroll().Right(),
                 m_TextArea2.VScroll().Bottom());

  // Fill the gap with the previous text lines
  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y - numLines + i;
    const DiffLine* pLeftLine = m_pDocument->LeftLine(lineIndex);
    const DiffLine* pRightLine = m_pDocument->RightLine(lineIndex);

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
                 m_TextArea1.VScroll().Left(),
                 m_TextArea1.VScroll().Top(),
                 m_TextArea1.VScroll().Right(),
                 m_TextArea1.VScroll().Bottom());

  ScrollRasterBF(m_pRPorts->Window(),
                 0,
                 numLines * m_FontHeight_pix,
                 m_TextArea2.VScroll().Left(),
                 m_TextArea2.VScroll().Top(),
                 m_TextArea2.VScroll().Right(),
                 m_TextArea2.VScroll().Bottom());

  for(size_t i = 0; i < numLines; i++)
  {
    int lineIndex = m_Y + m_MaxTextAreaLines + i;
    const DiffLine* pLeftLine = m_pDocument->LeftLine(lineIndex);
    const DiffLine* pRightLine = m_pDocument->RightLine(lineIndex);

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



void DiffWindow::paintDocuments(bool bFromStart)
{
  if(m_pDocument == NULL)
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

    const DiffLine* pLeftLine = m_pDocument->LeftLine(i);
    const DiffLine* pRightLine = m_pDocument->RightLine(i);

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

  if(!bHorizontallyScrolled && m_pDocument->LineNumbersEnabled())
  {
    //
    // Print the line numbers in left and right
    //

    // Move rastport cursor to start of left line numbers block
    ::Move(m_pRPorts->LineNum(),
           m_TextArea1.Left() + indent + 2,
           topEdge + m_TextArea1.Top() + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    const char* pLineNum = pLeftLine->LineNum();

    // Print left line's original line number
    Text(m_pRPorts->LineNum(), pLineNum, m_LineNumsWidth_chars);

    // Move rastport cursor to start of right line numbers block
    ::Move(m_pRPorts->LineNum(),
           m_TextArea2.Left() + indent + 2,
           topEdge + m_TextArea2.Top() + m_FontBaseline_pix + 1);

    // Get the text or set to empty spaces when there is none
    pLineNum = pRightLine->LineNum();

    // Print right line's original line number
    Text(m_pRPorts->LineNum(), pLineNum, m_LineNumsWidth_chars);
  }

  // Getting the RastPort for the left line to draw in. This depends on
  // the line background color which itself depends on the diff state
  // of the line.
  RastPort* pRPort = diffStateToRastPort(pLeftLine->State());

  long numCharsToPrint = calcNumPrintChars(pLeftLine, count, startIndex);
  if(numCharsToPrint > 0)
  {
    // Move rastport cursor to start of left line
    ::Move(pRPort,
          m_TextArea1.Left() + indent + m_LineNumsWidth_pix + 3,
          topEdge + m_TextArea1.Top() + m_FontBaseline_pix + 1);

    // Print left line
    Text(pRPort,
         pLeftLine->Text() + startIndex,
         numCharsToPrint);
  }

  // Getting the RastPort for the right line to draw in. This depends
  // on the line background color which itself depends on the diff
  // state of the line.
  pRPort = diffStateToRastPort(pRightLine->State());

  numCharsToPrint = calcNumPrintChars(pRightLine, count, startIndex);
  if(numCharsToPrint > 0)
  {
    // Move rastport cursor to start of right line
    ::Move(pRPort,
          m_TextArea2.Left()  + indent + m_LineNumsWidth_pix + 3,
          topEdge + m_TextArea2.Top() + m_FontBaseline_pix + 1);

    // Print the right line
    Text(pRPort,
         pRightLine->Text() + startIndex,
         numCharsToPrint);
  }
}
