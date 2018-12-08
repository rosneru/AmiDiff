#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include "DiffWindow.h"

DiffWindow::DiffWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort)
  : TextWindow(p_AppScreen, p_pMsgPort),
    m_pLeftDocument(NULL),
    m_pRightDocument(NULL),
    m_IndentX(5),
    m_IndentY(0),
    m_TextArea1Left(0),
    m_TextArea2Left(0),
    m_TextAreasTop(0),
    m_TextAreasWidth(0),
    m_TextAreasHeight(0),
    m_InnerWindowRight(0),
    m_InnerWindowBottom(0),
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

  // WORD widthDiff = m_pWindow->Width - m_WinWidth;
  // WORD heightDiff = m_pWindow->Height - m_WinHeight;

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
  if(TextWindow::Open(p_pMenuItemDisableAtOpen) == false)
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


  // Calculate some values which have to calculated after window
  // opening and after resizing
  calcSizes();

  // Paint the window decoration
  paintWindowDecoration();

  // Set main text pen for main diff/text display
  m_IntuiText.FrontPen  = m_AppScreen.Pens().Text();

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
  //      the m_LeftDocument.NumLines digits has. MAybe sprintf numLines
  //      to a buf and use this?
  m_LineNumbersWidth = TextLength(m_pWindow->RPort, "9999", 4);


  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Display the document titles above the text areas
  paintDocumentNames();

  // Display the first [1; m_MaxTextLines] lines
  paintDocument();

  // Paint the window decoration
  paintWindowDecoration();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
      PGA_Total, m_pLeftDocument->NumLines(),
      PGA_Top, 0,
      PGA_Visible, m_MaxTextLines,
      TAG_DONE);
  }


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
  int deltaLimit = m_MaxTextLines / 4;
/*
  if(deltaAbs < deltaLimit)
  {
    //
    // Scroll small amounts (1/4 window height) line by line
    //

    if(delta > 0)
    {
      scrollNLinesUp(deltaAbs);
    }
    else if(delta < 0)
    {
      scrollNLinesDown(deltaAbs);
    }

    return;
  }
*/
  //
  // Scroll bigger amounts by re-painting the whole page
  //

  // Determine how often getPrev or getNext must be called to set the
  // left and right documents to the new start point for page redrawing
  int numListTraverses = delta - m_MaxTextLines + 1;

  // Depending on numListTraverses call getPrev and getNext as often
  // as needed
  if(numListTraverses < 0)
  {
    for(int i = 0; i < -numListTraverses; i++)
    {
      m_pLeftDocument->GetPreviousLine();
      m_pRightDocument->GetPreviousLine();
      m_Y--;
    }

  }
  else if(numListTraverses > 0)
  {
    for(int i = 0; i < numListTraverses; i++)
    {
      m_pLeftDocument->GetNextLine();
      m_pRightDocument->GetNextLine();
      m_Y++;
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

void DiffWindow::initialize()
{
  // Call parent method to get to utilize scroll gadgets iside the
  // window borders
  TextWindow::initialize();

  // Set the default title
  SetTitle("DiffWindow");

  // Setting the window flags
  setFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DEPTHGADGET |     // Add a depth gadget
           WFLG_SIZEGADGET |      // Add a size gadget
           WFLG_GIMMEZEROZERO);   // Different layers for border and content

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_VANILLAKEY |     // Inform us about RAW key press
           IDCMP_RAWKEY |         // Inform us about printable key press
           IDCMP_CLOSEWINDOW |    // Inform us about click on close gadget
           IDCMP_NEWSIZE |        // Inform us about resizing
           IDCMP_IDCMPUPDATE);    // Inform us about TODO

  m_bInitialized = true;

}

void DiffWindow::calcSizes()
{
  // (Re-)calculate some values that may have be changed by re-sizing
  m_InnerWindowRight = m_pWindow->Width
    - m_AppScreen.IntuiScreen()->WBorLeft
    - m_SizeImageWidth;

  m_InnerWindowBottom = m_pWindow->Height
    - m_AppScreen.BarHeight()
    - m_SizeImageHeight;

  m_TextAreasWidth = m_InnerWindowRight - m_TextArea1Left - m_IndentX;
  m_TextAreasWidth /= 2;

  m_TextAreasHeight = m_InnerWindowBottom - m_TextAreasTop - m_IndentY;

  m_TextArea2Left = m_TextArea1Left + m_TextAreasWidth;

  if(m_AppScreen.FontHeight() == 0)
  {
    return;
  }

  // Calculates how many lines fit into current window size
  m_MaxTextLines = (m_TextAreasHeight - 4) /  m_AppScreen.FontHeight();

  // Set y-scroll-gadget's pot size in relation of new window size
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Visible, m_MaxTextLines,
    	TAG_DONE
	   );
  }

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

  while((pLeftLine != NULL) && (pRightLine !=NULL))
  {
    WORD lineNum = i - m_Y;

    paintLine(pLeftLine, pRightLine,
      lineNum * m_AppScreen.FontHeight());

    if(lineNum >= m_MaxTextLines - 1)
    {
      // Only display as many lines as fit into the window
      break;
    }

    i++;

    pLeftLine = m_pLeftDocument->GetNextLine();
    pRightLine = m_pRightDocument->GetNextLine();
  //   lineId++;
  //   pLine = m_pDocument->GetNextLine();
  }
}

void DiffWindow::paintLine(const SimpleString* p_pLeftLine,
    const SimpleString* p_pRightLine, WORD p_TopEdge)
{
  m_IntuiText.TopEdge = p_TopEdge;

  //
  // Print left line
  //
  m_IntuiText.BackPen = colorNameToPen(m_pLeftDocument->LineColor());
  m_IntuiText.IText = (UBYTE*)p_pLeftLine->C_str();
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_TextArea1Left + 3,
    m_TextAreasTop + 2);

  // Erase the area right of the left line.
  // This is made to avoid drawing into the right text area in case
  // left line is too long
  EraseRect(m_pWindow->RPort,
    m_TextArea2Left + 2,
    m_TextAreasTop + p_TopEdge + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + p_TopEdge + m_AppScreen.FontHeight() + 1);

  //
  // Print right line
  //
  m_IntuiText.BackPen = colorNameToPen(m_pRightDocument->LineColor());
  m_IntuiText.IText = (UBYTE*)p_pRightLine->C_str();
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_TextArea2Left + 3,
    m_TextAreasTop + 2);

  // Erase the area right of the right line
  // This is made to avoid drawing into the empty area in right of the
  // right text area in case right line is too long
  EraseRect(m_pWindow->RPort,
    m_TextArea2Left + m_TextAreasWidth,
    m_TextAreasTop + p_TopEdge + 2,
    m_InnerWindowRight,
    m_TextAreasTop + p_TopEdge + m_AppScreen.FontHeight() + 2);

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

bool DiffWindow::scrollDownOneLine()
{
  scrollNLinesDown(1);
  return true;
}

bool DiffWindow::scrollUpOneLine()
{
  scrollNLinesUp(1);
  return true;
}

void DiffWindow::scrollNLinesDown(int p_pNumLinesDown)
{
  // TODO remove after debugging
  YChangedHandler(m_Y-p_pNumLinesDown);
  return;


  if(p_pNumLinesDown < 1)
  {
    // Nothing to do
    return;
  }

  if(m_Y < 1)
  {
    // Do not move the text area downward if text is already at top
    return;
  }

  if(p_pNumLinesDown > m_Y)
  {
    // Scroll only as many lines as necessary
    p_pNumLinesDown = m_Y;
  }

  // Move text area downward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    -p_pNumLinesDown * m_AppScreen.FontHeight(),  // n * height
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 2);

  // fill the gap with the previous text lines
  for(int i = 0; i < p_pNumLinesDown; i++)
  {
    const SimpleString* pLeftLine = NULL;
    const SimpleString* pRightLine = NULL;

    getPreviousLineAtTop(pLeftLine, pRightLine);
    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    WORD lineNum = p_pNumLinesDown - i; // TODO add -1 ??

    paintLine(pLeftLine, pRightLine,
      lineNum * m_AppScreen.FontHeight());

    m_Y--;
  }

  // Repaint window decoration
  paintWindowDecoration();
}

void DiffWindow::scrollNLinesUp(int p_pNumLinesUp)
{
  // TODO remove after debugging
  YChangedHandler(m_Y+p_pNumLinesUp);
  return;

  if(p_pNumLinesUp < 1)
  {
    // Noting to do
    return;
  }

  if(m_pLeftDocument->NumLines() < m_MaxTextLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return;
  }

  if((m_Y + m_MaxTextLines) == m_pLeftDocument->NumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return;
  }

  if((m_Y + m_MaxTextLines + p_pNumLinesUp) > m_pLeftDocument->NumLines())
  {
    // Scroll only as many lines as necessary
    p_pNumLinesUp = m_pLeftDocument->NumLines() - (m_Y + m_MaxTextLines);
  }

  // Move text area upward by n * the height of one text line
  ScrollRaster(m_pWindow->RPort, 0,
    p_pNumLinesUp * m_AppScreen.FontHeight(),
    m_TextArea1Left + 3, m_TextAreasTop + 2,
    m_TextArea2Left + m_TextAreasWidth - 3,
    m_TextAreasTop + m_TextAreasHeight - 2);

  for(int i = 0; i < p_pNumLinesUp; i++)
  {
    const SimpleString* pLeftLine = NULL;
    const SimpleString* pRightLine = NULL;

    getNextLineAtBottom(pLeftLine, pRightLine);
    if(pLeftLine == NULL || pRightLine == NULL)
    {
      break;
    }

    WORD lineNum = m_MaxTextLines - p_pNumLinesUp + i; // TODO add -1 ??

    paintLine(pLeftLine, pRightLine,
      lineNum * m_AppScreen.FontHeight());

    m_Y++;
  }

  // Repaint window decoration
  paintWindowDecoration();
}




void DiffWindow::getPreviousLineAtTop(const SimpleString*& p_pLeftLine,
    const SimpleString*& p_pRightLine)
{
  if(m_LastScrollDirection == Downward)
  {
    p_pLeftLine = m_pLeftDocument->GetPreviousLine();
    p_pRightLine = m_pRightDocument->GetPreviousLine();
  }
  else
  {
    p_pLeftLine = m_pLeftDocument->GetIndexedLine(m_Y - 1);
    p_pRightLine = m_pRightDocument->GetIndexedLine(m_Y -1);

    m_LastScrollDirection = Downward;
  }
}

void DiffWindow::getNextLineAtBottom(const SimpleString*& p_pLeftLine,
    const SimpleString*& p_pRightLine)
{
  if(m_LastScrollDirection == Upward)
  {
    p_pLeftLine = m_pLeftDocument->GetNextLine();
    p_pRightLine = m_pRightDocument->GetNextLine();
  }
  else
  {
    p_pLeftLine = m_pLeftDocument->GetIndexedLine(m_Y + m_MaxTextLines);
    p_pRightLine = m_pRightDocument->GetIndexedLine(m_Y + m_MaxTextLines);
    m_LastScrollDirection = Upward;
  }
}
