#include <string.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include "TextWindow.h"

TextWindow::TextWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort)
  : WindowBase(p_AppScreen, p_pMsgPort),
    m_pDocument(NULL),
    m_MaxTextLines(0),
    m_Y(0),
    m_TextAreaLeft(0),
    m_TextAreaTop(0),
    m_TextAreaRight(0),
    m_TextAreaBottom(0),
    m_SizeImageWidth(18),
    m_SizeImageHeight(10),
    m_LastDocumentScrollDirection(None),
    m_pLeftArrowImage(NULL),
    m_pRightArrowImage(NULL),
    m_pUpArrowImage(NULL),
    m_pDownArrowImage(NULL),
    m_pXPropGadget(NULL),
    m_pYPropGadget(NULL),
    m_pLeftArrowButton(NULL),
    m_pRightArrowButton(NULL),
    m_pUpArrowButton(NULL),
    m_pDownArrowButton(NULL)
{

}

TextWindow::~TextWindow()
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


void TextWindow::Resized()
{
  if(m_pDocument == NULL)
  {
    return; // TODO
  }

  // Calculate how many lines *now* can be displayed in the window
  calcMaxWindowTextLines();

  if(m_pDocument->NumLines() == 0)
  {
    return;
  }

  // Set scroll gadgets pot size in relation of new window size
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Visible, m_MaxTextLines,
    	TAG_DONE
	   );
  }

  // Redraw obscured window regions
  Refresh();
}

void TextWindow::Refresh()
{
  BeginRefresh(m_pWindow);
  displayFile();
  EndRefresh(m_pWindow, TRUE);
}

bool TextWindow::Open(APTR p_pMenuItemDisableAtOpen)
{
  if(WindowBase::Open(p_pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  // Calculate values needed for text scrolling
  m_TextAreaRight = m_pWindow->Width; //- m_pWindow->BorderRight - m_TextAreaLeft;
  m_TextAreaBottom = m_pWindow->Height; // - m_pWindow->BorderBottom - m_TextAreaTop;

  // Calculate how many lines can be displayed in the window
  calcMaxWindowTextLines();

  m_TextAttr.ta_Name = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_Flags;

  // Prepare IntuiText for line-by-line printing
  m_IntuiText.DrawMode  = JAM2;
  m_IntuiText.LeftEdge  = 0;
  m_IntuiText.TopEdge   = 0;
  m_IntuiText.ITextFont = &m_TextAttr;
  m_IntuiText.NextText  = NULL;

  return true;
}


bool TextWindow::SetContent(TextDocument* p_pTextDocument)
{
  m_pDocument = p_pTextDocument;
  m_Y = 0;

  if(!IsOpen())
  {
    return true;
  }

  // Clear the window completely
  SetRast(m_pWindow->RPort,1L);

  // Set full path of opened file as window title
  SetTitle(p_pTextDocument->FileName());

  // Display the first [1; m_MaxTextLines] lines
  displayFile();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Total, m_pDocument->NumLines(),
    	PGA_Top, 0,
    	PGA_Visible, m_MaxTextLines,
    	TAG_DONE
	   );
  }

  return true;
}

void TextWindow::YChangedHandler(size_t p_NewY)
{
  // set the new y-position
  m_Y = p_NewY;

  // Clear the window completely
  EraseRect(m_pWindow->RPort,
    m_TextAreaLeft, m_TextAreaTop, m_TextAreaRight, m_TextAreaBottom);

  // Display the beginning at new y-position
  displayFile();
}


void TextWindow::YIncrease()
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


void TextWindow::YDecrease()
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


void TextWindow::initialize()
{
  //
  // Setting up scroll bars and gadgets for the window. They will be
  // attached to the window at opening time
  //
  ULONG imageWidth = 0;   // to successfully store the other images widths
  ULONG imageHeight = 0;  // to successfully store the other images heights

  // Getting the width and height of the current system size gadget
  struct Image* pSizeImage = createImageObj(
    SIZEIMAGE, m_SizeImageWidth, m_SizeImageHeight);

  // the size image is only needed for getting its width and height so
  // it can be disposed right now
  if(pSizeImage != NULL)
  {
    DisposeObject(pSizeImage);
    pSizeImage = NULL;
  }

  // Creating the arrow down image and getting its width and height
  m_pDownArrowImage = createImageObj(DOWNIMAGE, imageWidth, imageHeight);

  // Creating the arrow down gadget
  m_pDownArrowButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_ID, GID_ArrowDown,
    GA_RelRight, -imageWidth+1,
    GA_RelBottom, -m_SizeImageHeight-imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_RightBorder, TRUE,
    GA_Image, m_pDownArrowImage,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the arrow up image and getting its width and height
  m_pUpArrowImage = createImageObj(UPIMAGE, imageWidth, imageHeight);

  // Creating the arrow down gadget
  m_pUpArrowButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pDownArrowButton,
    GA_ID, GID_ArrowUp,
    GA_RelRight, -imageWidth+1,
    GA_RelBottom, -m_SizeImageHeight-imageHeight-imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_RightBorder, TRUE,
    GA_Image, m_pUpArrowImage,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the vertical proportional gadget / slider
	m_pYPropGadget = (struct Gadget*) NewObject(
	  NULL, PROPGCLASS,
  	GA_Previous, m_pUpArrowButton,
  	GA_ID, GID_PropY,
  	GA_RelRight, -m_SizeImageWidth+4,
  	GA_Top, m_AppScreen.BarHeight(),
  	GA_Width, m_SizeImageWidth-6,
  	GA_RelHeight, -m_SizeImageHeight-imageHeight-imageHeight-m_AppScreen.BarHeight()-1,
  	GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
  	GA_GZZGadget, TRUE,
  	GA_RightBorder, TRUE,
  	PGA_Freedom, FREEVERT,
  	PGA_Borderless, TRUE,
  	PGA_NewLook, TRUE,
  	PGA_Total, 100,
  	PGA_Top, 0, // TODO remove??
  	PGA_Visible, 100,
  	ICA_TARGET, ICTARGET_IDCMP,
  	TAG_END);

  // Creating the arrow left image and getting its width and height
  m_pRightArrowImage = createImageObj(RIGHTIMAGE, imageWidth, imageHeight);

  // Creating the arrow right gadget
  m_pRightArrowButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pYPropGadget,
    GA_ID, GID_ArrowRight,
    GA_RelRight, -m_SizeImageWidth-imageWidth+1,
    GA_RelBottom, -imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_BottomBorder, TRUE,
    GA_Image, m_pRightArrowImage,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the arrow left image and getting its width and height
  m_pLeftArrowImage = createImageObj(LEFTIMAGE, imageWidth, imageHeight);

  // Creating the arrow left gadget
  m_pLeftArrowButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pRightArrowButton,
    GA_ID, GID_ArrowLeft,
    GA_RelRight, -m_SizeImageWidth-imageWidth-imageWidth+1,
    GA_RelBottom, -imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_BottomBorder, TRUE,
    GA_Image, m_pLeftArrowImage,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the horizontal proportional gadget / slider
  m_pXPropGadget = (struct Gadget*) NewObject(
    NULL, PROPGCLASS,
    GA_Previous, m_pLeftArrowButton,
    GA_ID, GID_PropX,
    GA_Left, m_AppScreen.IntuiScreen()->WBorLeft,
    GA_RelBottom, -m_SizeImageHeight+3,
    GA_RelWidth, -m_SizeImageWidth-imageWidth-imageWidth-m_AppScreen.IntuiScreen()->WBorLeft-1,
    GA_Height, m_SizeImageHeight-4,
    GA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_BottomBorder, TRUE,
    PGA_Freedom, FREEHORIZ,
    PGA_Borderless, TRUE,
    PGA_NewLook, TRUE,
    PGA_Total, 100,
    //PGA_Left, 0,  // TODO remove??
    PGA_Visible, 100,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Set the default title
  SetTitle("TextWindow");

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
  setFirstGadget(m_pDownArrowButton);

  m_bInitialized = true;

}


void TextWindow::calcMaxWindowTextLines()
{
  if(m_AppScreen.FontHeight() == 0)
  {
    return;
  }

  m_MaxTextLines = m_pWindow->Height;
  m_MaxTextLines -= m_pWindow->BorderTop;
  m_MaxTextLines -= m_pWindow->BorderBottom;
  m_MaxTextLines -= m_TextAreaTop;
  m_MaxTextLines /= m_AppScreen.FontHeight();
}


void TextWindow::displayLine(const SimpleString* p_pLine, WORD p_TopEdge)
{
  m_IntuiText.IText = (UBYTE*)p_pLine->C_str();
  m_IntuiText.TopEdge = p_TopEdge;
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_TextAreaLeft, m_TextAreaTop);
}

void TextWindow::displayFile()
{
  if(m_pDocument == NULL)
  {
    return;
  }

  size_t lineId = m_Y;
  const SimpleString* pLine = m_pDocument->GetIndexedLine(lineId);
  while(pLine != NULL)
  {
    displayLine(pLine, (lineId - m_Y ) * m_AppScreen.FontHeight());

    if((lineId - m_Y) >= m_MaxTextLines - 1)
    {
      // Only display as many lines as fit into the window
      break;
    }

    lineId++;
    pLine = m_pDocument->GetNextLine();
  }
}

bool TextWindow::scrollUpOneLine()
{
  if(m_pDocument->NumLines() < m_MaxTextLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return false;
  }

  if((m_Y + m_MaxTextLines) == m_pDocument->NumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return false;
  }

  // Scroll upward one line by the current font height
  ScrollRaster(m_pWindow->RPort, 0, m_AppScreen.FontHeight(),
    m_TextAreaLeft, m_TextAreaTop, m_TextAreaRight, m_TextAreaBottom);

  // Get the line which at current scroll position has to be printed as
  // the windows last line
  const SimpleString* pLine = NULL;
  if(m_LastDocumentScrollDirection == NextLine)
  {
    pLine = m_pDocument->GetNextLine();
  }
  else
  {
    pLine = m_pDocument->GetIndexedLine(m_Y + m_MaxTextLines);
    m_LastDocumentScrollDirection = NextLine;
  }

  if(pLine == NULL)
  {
    return false;
  }

  m_Y++;

  // Print the new last line
  displayLine(pLine, (m_MaxTextLines - 1) * m_AppScreen.FontHeight());
  return true;
}

bool TextWindow::scrollDownOneLine()
{
  if(m_Y < 1)
  {
    // Do not move the scroll area downward if text is already at top
    return false;
  }

  // Move scroll area downward by the height of one text line
  ScrollRaster(m_pWindow->RPort, 0, -m_AppScreen.FontHeight(),
    m_TextAreaLeft, m_TextAreaTop, m_TextAreaRight, m_TextAreaBottom);

  // Delete the possible visible line below the scroll area which can
  // be caused by the scroll operation above
  EraseRect(m_pWindow->RPort,
    m_TextAreaLeft, m_MaxTextLines * m_AppScreen.FontHeight(),
    m_TextAreaRight, m_TextAreaBottom);

  // Get the line which at current scroll position has to be printed as
  // the windows first line
  const SimpleString* pLine = NULL;
  if(m_LastDocumentScrollDirection == PreviousLine)
  {
    pLine = m_pDocument->GetPreviousLine();
  }
  else
  {
    pLine = m_pDocument->GetIndexedLine(m_Y - 1);
    m_LastDocumentScrollDirection = PreviousLine;
  }

  if(pLine == NULL)
  {
    return false;
  }

  m_Y--;

  // Print the new first line
  displayLine(pLine, 0);
  return true;
}


void TextWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(!IsOpen())
  {
    return;
  }

  switch (p_Class)
  {
    case IDCMP_IDCMPUPDATE:
    {
      ULONG tagData = GetTagData(GA_ID, 0,
        (struct TagItem *)p_IAddress);
      switch(tagData)
      {
        case TextWindow::GID_PropY:
        {
          size_t newY = GetTagData(PGA_Top, 0, (struct TagItem *)
            p_IAddress);

          YChangedHandler(newY);
          break;
        }

        case TextWindow::GID_ArrowUp:
        {
          YDecrease();
          break;
        }

        case TextWindow::GID_ArrowDown:
        {
          YIncrease();
          break;
        }

      }
      break;
    }

    case IDCMP_RAWKEY:
    {
      if(p_Code == CURSORDOWN)
      {
        YIncrease();
      }
      else if(p_Code == CURSORUP)
      {
        YDecrease();
      }
      break;
    }

    case IDCMP_NEWSIZE:
    {
      Resized();
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      Refresh();
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      Close();
      break;
    }
  }
}
