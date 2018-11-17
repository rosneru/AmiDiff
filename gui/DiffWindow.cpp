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
    m_pLeftDiffDocument(NULL),
    m_pRightDiffDocument(NULL),
    m_IndentX(30),
    m_IndentY(30),
    m_TextAreaLeft(0),
    m_TextAreaTop(0),
    m_TextAreaWidth(0),
    m_TextAreaHeight(0),
    m_InnerWindowRight(0),
    m_InnerWindowBottom(0)
{

}

DiffWindow::~DiffWindow()
{
  Close();

  if(m_pLeftArrowButton != NULL)
  {
    DisposeObject(m_pLeftArrowButton);
    m_pLeftArrowButton = NULL;
  }

  if(m_pLeftArrowImage != NULL)
  {
    DisposeObject(m_pLeftArrowImage);
    m_pLeftArrowImage = NULL;
  }

  if(m_pRightArrowButton != NULL)
  {
    DisposeObject(m_pRightArrowButton);
    m_pRightArrowButton = NULL;
  }

  if(m_pRightArrowImage != NULL)
  {
    DisposeObject(m_pRightArrowImage);
    m_pRightArrowImage = NULL;
  }

  if(m_pXPropGadget != NULL)
  {
    DisposeObject(m_pXPropGadget);
    m_pXPropGadget = NULL;
  }

  if(m_pDownArrowButton != NULL)
  {
    DisposeObject(m_pDownArrowButton);
    m_pDownArrowButton = NULL;
  }

  if(m_pDownArrowImage != NULL)
  {
    DisposeObject(m_pDownArrowImage);
    m_pDownArrowImage = NULL;
  }

  if(m_pUpArrowButton != NULL)
  {
    DisposeObject(m_pUpArrowButton);
    m_pUpArrowButton = NULL;
  }

  if(m_pUpArrowImage != NULL)
  {
    DisposeObject(m_pUpArrowImage);
    m_pUpArrowImage = NULL;
  }

  if(m_pYPropGadget != NULL)
  {
    DisposeObject(m_pYPropGadget);
    m_pYPropGadget = NULL;
  }
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

  WORD widthDiff = m_pWindow->Width - m_WinWidth;
  WORD heightDiff = m_pWindow->Height - m_WinHeight;

  m_WinWidth = m_pWindow->Width;
  m_WinHeight = m_pWindow->Height;

  // Calculate how many lines *now* can be displayed in the window
  calcMaxWindowTextLines();

  // Set scroll gadgets pot size in relation of new window size
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Visible, m_MaxTextLines,
    	TAG_DONE
	   );
  }

  draw(widthDiff, heightDiff);
}

void DiffWindow::Refresh()
{
  BeginRefresh(m_pWindow);

  draw();
  displayFile();

  EndRefresh(m_pWindow, TRUE);
}

bool DiffWindow::Open(APTR p_pMenuItemDisableAtOpen)
{
  if(TextWindow::Open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  draw();
  return true;
}

bool DiffWindow::SetContent(DiffDocument* p_pLeftDiffDocument,
    DiffDocument* p_pRightDiffDocument)
{
  m_pLeftDiffDocument = p_pLeftDiffDocument;
  m_pRightDiffDocument = p_pRightDiffDocument;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // !!TODO!!

  // // Clear the window completely
  // EraseRect(m_pWindow->RPort,
  //   m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);


  // // Set full path of opened file as window title
  // SetTitle(p_pTextDocument->FileName());
  // displayFile();

  // // Set scroll gadgets pot size dependent on window size and the number
  // // of lines in opened file
  // if(m_pYPropGadget != NULL)
  // {
	//   SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
  //   	PGA_Total, m_pDocument->NumLines(),
  //   	PGA_Top, 0,
  //   	PGA_Visible, m_MaxTextLines,
  //   	TAG_DONE
	//    );
  // }

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

  // Setting the first gadget of the gadet list for the window
//  setFirstGadget(m_pDownArrowButton);

  m_TextAreaLeft = m_IndentX;
  m_TextAreaTop = m_IndentY;

  m_bInitialized = true;

}

void DiffWindow::draw(WORD p_WidthDiff, WORD p_HeightDiff)
{
  // Erase old rectbefore re-calculating and re-drawing it
  EraseRect(m_pWindow->RPort,
    0, 0, m_InnerWindowRight, m_InnerWindowBottom);

  m_InnerWindowRight = m_pWindow->Width
    - m_AppScreen.IntuiScreen()->WBorLeft
    - m_SizeImageWidth;

  m_InnerWindowBottom = m_pWindow->Height
    - m_AppScreen.BarHeight()
    - m_AppScreen.IntuiScreen()->WBorBottom;

  m_TextAreaWidth = m_InnerWindowRight - m_TextAreaLeft - m_IndentX;
  m_TextAreaHeight = m_InnerWindowBottom - m_TextAreaTop - m_IndentY;

  DrawBevelBox(m_pWindow->RPort,
    m_TextAreaLeft, m_TextAreaTop,
    m_TextAreaWidth, m_TextAreaHeight,
    GT_VisualInfo, m_AppScreen.GadtoolsVisualInfo(),
    GTBB_Recessed, TRUE,
    TAG_DONE);

}

void DiffWindow::calcMaxWindowTextLines()
{
  m_MaxTextLines = m_pWindow->Height;
  m_MaxTextLines -= m_pWindow->BorderTop;
  m_MaxTextLines -= m_pWindow->BorderBottom;
  m_MaxTextLines -= m_ScrollYMin;
  m_MaxTextLines /= m_AppScreen.FontHeight();
}


void DiffWindow::displayLine(const SimpleString* p_pLine, WORD p_TopEdge)
{
  m_IntuiText.IText = (UBYTE*)p_pLine->C_str();
  m_IntuiText.TopEdge = p_TopEdge;
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_ScrollXMin, m_ScrollYMin);
}

void DiffWindow::displayFile()
{
  if(m_pLeftDiffDocument == NULL || m_pRightDiffDocument == NULL)
  {
    return; // TODO
  }

  // !!TODO!!

  // size_t lineId = m_Y;
  // const SimpleString* pLine = m_pDocument->GetIndexedLine(lineId);
  // while(pLine != NULL)
  // {
  //   displayLine(pLine, (lineId - m_Y ) * m_FontHeight);

  //   if((lineId - m_Y) >= m_MaxTextLines - 1)
  //   {
  //     // Only display as many lines as fit into the window
  //     break;
  //   }

  //   lineId++;
  //   pLine = m_pDocument->GetNextLine();
  // }
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
  // displayLine(pLine, (m_MaxTextLines - 1) * m_FontHeight);
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
  // displayLine(pLine, 0);
  return true;
}
