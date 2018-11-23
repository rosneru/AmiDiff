#include <string.h>
#include <stdio.h>

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
    m_IndentY(2 * p_AppScreen.FontHeight()),
    m_TextArea1Left(0),
    m_TextArea2Left(0),
    m_TextAreaTop(0),
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
  if(m_pWindow == NULL)
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

  // Calculate how many lines *now* can be displayed in the window
  calcMaxWindowTextLines();

  // Calculate some values which have to calculated after window 
  // opening and after resizing
  calcSizes();


  // Paint the window decoration
  paintWindowDecoration();

  if((m_pLeftDocument == NULL) || m_pRightDocument == NULL)
  {
    return;
  }

  // Paint the document names
  paintDocumentNames();

  // Set scroll gadgets pot size in relation of new window size
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Visible, m_MaxTextLines,
    	TAG_DONE
	   );
  }

  // TODO paintFile
}

void DiffWindow::Refresh()
{
  BeginRefresh(m_pWindow);

  // Paint the window decoration
  paintWindowDecoration();
  paintFile();

  EndRefresh(m_pWindow, TRUE);
}

bool DiffWindow::Open(APTR p_pMenuItemDisableAtOpen)
{
  if(TextWindow::Open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  //
  // Calculate some initial values which only have to be calculated 
  // unique
  //
  
  // TODO Remove as soon a constructor initilization works.
  //  AND then m_IndentY can be made const
  //m_IndentY = 2 * m_AppScreen.FontHeight();

  m_TextArea1Left = m_IndentX;
  m_TextAreaTop = m_IndentY;


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


  // Display the document titles above the text areas
  paintDocumentNames();

  // Clear the window completely
  SetRast(m_pWindow->RPort, m_AppScreen.Pens().Background());

  // Display the first [1; m_MaxTextLines] lines
  paintFile();

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

  // Paint the window decoration
  paintWindowDecoration();

  return true;
}

void DiffWindow::YChangedHandler(size_t p_NewY)
{
  // set the new y-position
  m_Y = p_NewY;

  // Clear the window completely
  EraseRect(m_pWindow->RPort,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  // Display the beginning at new y-position
  displayFile();
}


void DiffWindow::YIncrease()
{
  // Scroll the text
  if(scrollUpOneLine() == false)
  {
    // No scrolling possible
    return;
  }

  // Increase scroll gadgets TOP value
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Top, m_Y,
    	TAG_DONE
	   );
  }
}


void DiffWindow::YDecrease()
{
  // Scroll the text
  if(scrollDownOneLine() == false)
  {
    // No scrolling possible
    return;
  }

  // Decrease scroll gadgets TOP value
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Top, m_Y,
    	TAG_DONE
	   );
  }
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
           IDCMP_REFRESHWINDOW |  // Inform us when refreshing is necessary
           IDCMP_IDCMPUPDATE);    // Inform us about TODO

  m_bInitialized = true;

}

void DiffWindow::calcMaxWindowTextLines()
{
  if(m_AppScreen.FontHeight() == 0)
  {
    return;
  }

  m_MaxTextLines = m_TextAreasHeight /  m_AppScreen.FontHeight();
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

  m_TextAreasHeight = m_InnerWindowBottom - m_TextAreaTop - m_IndentY;

  m_TextArea2Left = m_TextArea1Left + m_TextAreasWidth;
}

void DiffWindow::paintFile()
{
  if(m_pLeftDocument == NULL || m_pRightDocument == NULL)
  {
    return; // TODO
  }

  size_t i = m_Y;
  const SimpleString* pLeftLine = m_pLeftDocument->GetIndexedLine(i);
  const SimpleString* pRightLine = m_pRightDocument->GetIndexedLine(i);
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
    m_TextAreaTop + 2);

  //
  // Print right line
  //
  m_IntuiText.BackPen = colorNameToPen(m_pRightDocument->LineColor());
  m_IntuiText.IText = (UBYTE*)p_pRightLine->C_str();
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_TextArea2Left + 3,
    m_TextAreaTop + 2);
}

void DiffWindow::paintWindowDecoration()
{
/*
  // Erase old rect before re-calculating and re-drawing it
  EraseRect(m_pWindow->RPort,
    0, 0, m_InnerWindowRight, m_InnerWindowBottom);
*/

  // Create borders for the two text areas
  DrawBevelBox(m_pWindow->RPort,
    m_TextArea1Left, m_TextAreaTop,
    m_TextAreasWidth, m_TextAreasHeight,
    GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);

  DrawBevelBox(m_pWindow->RPort,
    m_TextArea2Left, m_TextAreaTop,
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

  intuiText.TopEdge   = m_TextAreaTop - m_AppScreen.FontHeight() - 1;

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


bool DiffWindow::scrollUpOneLine()
{
  // !!TODO!!

  // if(m_pDocument->NumLines() < m_MaxTextLines)
  // {
  //   // Do not move the scroll area upward if all the text fits into
  //   // the window
  //   return false;
  // }

  // if((m_Y + m_MaxTextLines) == m_pDocument->NumLines())
  // {
  //   // Do not move the scroll area upward if text already at bottom
  //   return false;
  // }

  // // Scroll upward one line by the current font height
  // ScrollRaster(m_pWindow->RPort, 0, m_FontHeight,
  //   m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  // // Get the line which at current scroll position has to be printed as
  // // the windows last line
  // const SimpleString* pLine = NULL;
  // if(m_LastScrollDirection == Upward)
  // {
  //   pLine = m_pDocument->GetNextLine();
  // }
  // else
  // {
  //   pLine = m_pDocument->GetIndexedLine(m_Y + m_MaxTextLines);
  //   m_LastScrollDirection = Upward;
  // }

  // if(pLine == NULL)
  // {
  //   return false;
  // }

  // m_Y++;

  // // Print the new last line
  // paintLine(pLine, (m_MaxTextLines - 1) * m_FontHeight);
  return true;
}

bool DiffWindow::scrollDownOneLine()
{
  // !!TODO!!

  // if(m_Y < 1)
  // {
  //   // Do not move the scroll area downward if text is already at top
  //   return false;
  // }

  // // Move scroll area downward by the height of one text line
  // ScrollRaster(m_pWindow->RPort, 0, -m_FontHeight,
  //   m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  // // Delete the possible visible line below the scroll area which can
  // // be caused by the scroll operation above
  // EraseRect(m_pWindow->RPort,
  //   m_ScrollXMin, m_MaxTextLines * m_FontHeight,
  //   m_ScrollXMax, m_ScrollYMax);

  // // Get the line which at current scroll position has to be printed as
  // // the windows first line
  // const SimpleString* pLine = NULL;
  // if(m_LastScrollDirection == Downward)
  // {
  //   pLine = m_pDocument->GetPreviousLine();
  // }
  // else
  // {
  //   pLine = m_pDocument->GetIndexedLine(m_Y - 1);
  //   m_LastScrollDirection = Downward;
  // }

  // if(pLine == NULL)
  // {
  //   return false;
  // }

  // m_Y--;

  // // Print the new first line
  // paintLine(pLine, 0);
  return true;
}
