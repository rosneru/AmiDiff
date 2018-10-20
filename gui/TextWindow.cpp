#include <string.h>

#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include "TextWindow.h"

TextWindow::TextWindow(AppScreen* p_pAppScreen)
  : WindowBase(p_pAppScreen),
    m_pDocument(NULL),
    m_MaxWindowTextLines(0),
    m_Y(0),
    m_FontHeight(0),
    m_ScrollXMin(0),
    m_ScrollYMin(0),
    m_ScrollXMax(0),
    m_ScrollYMax(0),
    m_LastScrollDirection(None),
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
  //
  // Calculate some basic values
  //
  m_FontHeight = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
	int barHeight = m_pAppScreen->IntuiScreen()->WBorTop + m_FontHeight + 2;

  //
  // Setting up scroll bars and gadgets for the window. They will be
  // attached to the window at opening time
  //
  ULONG sizeImageWidth = 18;  // the width of the size image
  ULONG sizeImageHeight = 10; // the height of the size image
  ULONG imageWidth = 0;   // to successfully store the other images widths
  ULONG imageHeight = 0;  // to successfully store the other images heights

  // Getting the width and height of the current system size gadget
  struct Image* pSizeImage = createImageObj(
    SIZEIMAGE, sizeImageWidth, sizeImageHeight);

  // the size image is only needed for getting its width and height so
  // it can be disposed right now
  if(pSizeImage != NULL)
  {
    DisposeObject(pSizeImage);
  }

  // Creating the arrow down image and getting its width and height
  m_pDownArrowImage = createImageObj(DOWNIMAGE, imageWidth, imageHeight);

  // Creating the arrow down gadget
  m_pDownArrowButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_ID, DGID_DOWNARROW,
    GA_RelRight, -imageWidth+1,
    GA_RelBottom, -sizeImageHeight-imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
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
    GA_ID, DGID_UPARROW,
    GA_RelRight, -imageWidth+1,
    GA_RelBottom, -sizeImageHeight-imageHeight-imageHeight+1,
    GA_Width, imageWidth,
    GA_Height, imageHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_GZZGadget, TRUE,
    GA_RightBorder, TRUE,
    GA_Image, m_pUpArrowImage,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the vertical proportional gadget / slider
	m_pYPropGadget = (struct Gadget*) NewObject(
	  NULL, PROPGCLASS,
  	GA_Previous, m_pUpArrowButton,
  	GA_ID, DGID_YPROP,
  	GA_RelRight, -sizeImageWidth+4,
  	GA_Top, barHeight,
  	GA_Width, sizeImageWidth-6,
  	GA_RelHeight, -sizeImageHeight-imageHeight-imageHeight-barHeight-1,
  	GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
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
      GA_ID, DGID_RIGHTARROW,
      GA_RelRight, -sizeImageWidth-imageWidth+1,
      GA_RelBottom, -imageHeight+1,
      GA_Width, imageWidth,
      GA_Height, imageHeight,
      GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
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
      GA_ID, DGID_LEFTARROW,
      GA_RelRight, -sizeImageWidth-imageWidth-imageWidth+1,
      GA_RelBottom, -imageHeight+1,
      GA_Width, imageWidth,
      GA_Height, imageHeight,
      GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
      GA_GZZGadget, TRUE,
      GA_BottomBorder, TRUE,
      GA_Image, m_pLeftArrowImage,
      ICA_TARGET, ICTARGET_IDCMP,
      TAG_END);

    // Creating the horizontal proportional gadget / slider
    m_pXPropGadget = (struct Gadget*) NewObject(
      NULL, PROPGCLASS,
      GA_Previous, m_pLeftArrowButton,
      GA_ID, DGID_XPROP,
      GA_Left, m_pAppScreen->IntuiScreen()->WBorLeft,
      GA_RelBottom, -sizeImageHeight+3,
      GA_RelWidth, -sizeImageWidth-imageWidth-imageWidth-m_pAppScreen->IntuiScreen()->WBorLeft-1,
      GA_Height, sizeImageHeight-4,
      GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
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
}

TextWindow::~TextWindow()
{
  Close();
}


void TextWindow::HandleIdcmp(struct IntuiMessage* p_pMsg)
{

}

void TextWindow::Resized()
{
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
    	PGA_Visible, m_MaxWindowTextLines,
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

bool TextWindow::Open()
{
  //
  // Initial validations
  //

  if(m_pAppScreen == NULL)
  {
    // In this design we need a screen to open the window
    return false;
  }

  if(m_pWindow != NULL)
  {
    // Not opening the window if it is already open
    return false;
  }

  //
  // Calculating window size etc in dependency of screen dimensions
  //
  int screenWidth = m_pAppScreen->IntuiScreen()->Width;
  int screenHeight = m_pAppScreen->IntuiScreen()->Height;
  int screenBarHeight = m_pAppScreen->IntuiScreen()->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  int activateWin = TRUE;
  int winLeft = 0;
  m_Title = "Left Diff Window";

  winLeft = screenWidth / 4;
/*
  if(p_DwType == RIGHT)
  {
    winLeft = winWidth; // begin one pixel after LEFT window ends
    m_Title = "Right Diff Window";
    m_FileRequesterTitle = "Select right diff file";
    activateWin = FALSE;
  }
*/
  //
  // Opening the window
  //
  m_pWindow = OpenWindowTags(NULL,
    WA_Left, winLeft,
    WA_Top, screenBarHeight + 1,
    WA_Width, winWidth,
    WA_Height, winHeight,
    WA_Title, (ULONG) m_Title.C_str(),
    WA_Activate, activateWin,
    WA_PubScreen, (ULONG) m_pAppScreen->IntuiScreen(),
    WA_IDCMP, IDCMP_MENUPICK |      // Inform us about menu selection
              IDCMP_VANILLAKEY |    // Inform us about RAW key press
              IDCMP_RAWKEY |        // Inform us about printable key press
              IDCMP_CLOSEWINDOW |   // Inform us about click on close gadget
              IDCMP_NEWSIZE |       // Inform us about resizing
              IDCMP_REFRESHWINDOW | // Inform us when refreshing is necessary
              IDCMP_IDCMPUPDATE,    // Inform us about TODO
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Flags, WFLG_CLOSEGADGET |    // Add a close gadget
              WFLG_DRAGBAR |        // Add a drag gadget
              WFLG_DEPTHGADGET |    // Add a depth gadget
              WFLG_SIZEGADGET |     // Add a size gadget
              WFLG_GIMMEZEROZERO |  // Different layers for border and content
              WFLG_ACTIVATE,
    WA_SimpleRefresh, TRUE,
		WA_MinWidth, 120,
		WA_MinHeight, 90,
		WA_MaxWidth, -1,
		WA_MaxHeight, -1,
    WA_Gadgets, m_pDownArrowButton,
    TAG_END);

  // Calculate values needed for text scrolling
  m_ScrollXMax = m_pWindow->Width; //- m_pWindow->BorderRight - m_ScrollXMin;
  m_ScrollYMax = m_pWindow->Height; // - m_pWindow->BorderBottom - m_ScrollYMin;

  // Calculate how many lines can be displayed in the window
  calcMaxWindowTextLines();

  // Setup structs for text drawing
    // Setup Pens, TextAttr and prepare IntuiText
  // TODO Remove it to some better place
  ULONG txtPen = m_pAppScreen->IntuiDrawInfo()->dri_Pens[TEXTPEN];
  ULONG bgPen = m_pAppScreen->IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];

  m_TextAttr.ta_Name = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_YSize = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_Flags;

  // Prepare IntuiText for line-by-line printing
  m_IntuiText.FrontPen  = txtPen;
  m_IntuiText.BackPen   = bgPen;
  m_IntuiText.DrawMode  = JAM2;
  m_IntuiText.LeftEdge  = 0;
  m_IntuiText.TopEdge   = 0;
  m_IntuiText.ITextFont = &m_TextAttr;
  m_IntuiText.NextText  = NULL;

  return true;
}

void TextWindow::Close()
{
  if(m_pLeftArrowButton != NULL)
  {
    DisposeObject(m_pLeftArrowButton);
  }

  if(m_pLeftArrowImage != NULL)
  {
    DisposeObject(m_pLeftArrowImage);
  }

  if(m_pRightArrowButton != NULL)
  {
    DisposeObject(m_pRightArrowButton);
  }

  if(m_pRightArrowImage != NULL)
  {
    DisposeObject(m_pRightArrowImage);
  }

  if(m_pXPropGadget != NULL)
  {
    DisposeObject(m_pXPropGadget);
  }

  if(m_pDownArrowButton != NULL)
  {
    DisposeObject(m_pDownArrowButton);
  }

  if(m_pDownArrowImage != NULL)
  {
    DisposeObject(m_pDownArrowImage);
  }

  if(m_pUpArrowButton != NULL)
  {
    DisposeObject(m_pUpArrowButton);
  }

  if(m_pUpArrowImage != NULL)
  {
    DisposeObject(m_pUpArrowImage);
  }

  if(m_pYPropGadget != NULL)
  {
    DisposeObject(m_pYPropGadget);
  }

  // Also call Close() in parent
  // TODO debug if it really happens
  WindowBase::Close();
}


bool TextWindow::SetContent(TextDocument* p_pTextDocument)
{
  char* pFn[] = { p_pTextDocument->FileName().C_str() };
  VPrintf("Dateiname = %s\n", pFn);
  return true;

  m_Y = 0;

  // Clear the window completely
  EraseRect(m_pWindow->RPort,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);


  // Set full path of opened file as window title
  SetTitle(p_pTextDocument->FileName());
  displayFile();

  // Set scroll gadgets pot size dependent on window size and the number
  // of lines in opened file
  if(m_pYPropGadget != NULL)
  {
	  SetGadgetAttrs(m_pYPropGadget, m_pWindow, NULL,
    	PGA_Total, m_pDocument->NumLines(),
    	PGA_Top, 0,
    	PGA_Visible, m_MaxWindowTextLines,
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
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

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


void TextWindow::calcMaxWindowTextLines()
{
  m_MaxWindowTextLines = m_pWindow->Height;
  m_MaxWindowTextLines -= m_pWindow->BorderTop;
  m_MaxWindowTextLines -= m_pWindow->BorderBottom;
  m_MaxWindowTextLines -= m_ScrollYMin;
  m_MaxWindowTextLines /= m_FontHeight;
}


void TextWindow::displayLine(SimpleString* p_pLine, WORD p_TopEdge)
{
  m_IntuiText.IText = (UBYTE*)p_pLine->C_str();
  m_IntuiText.TopEdge = p_TopEdge;
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_ScrollXMin, m_ScrollYMin);
}

void TextWindow::displayFile()
{
  size_t lineId = m_Y;
  SimpleString* pLine = m_pDocument->GetIndexedLine(lineId);
  while(pLine != NULL)
  {
    displayLine(pLine, (lineId - m_Y ) * m_FontHeight);

    if((lineId - m_Y) >= m_MaxWindowTextLines - 1)
    {
      break;
    }

    lineId++;
    pLine = m_pDocument->GetNextLine();
  }
}

bool TextWindow::scrollUpOneLine()
{
  if(m_pDocument->NumLines() < m_MaxWindowTextLines)
  {
    // Do not move the scroll area upward if all the text fits into
    // the window
    return false;
  }

  if((m_Y + m_MaxWindowTextLines) == m_pDocument->NumLines())
  {
    // Do not move the scroll area upward if text already at bottom
    return false;
  }

  // Scroll upward one line by the current font height
  ScrollRaster(m_pWindow->RPort, 0, m_FontHeight,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  // Get the line which at current scroll position has to be printed as
  // the windows last line
  SimpleString* pLine = NULL;
  if(m_LastScrollDirection == Upward)
  {
    pLine = m_pDocument->GetNextLine();
  }
  else
  {
    pLine = m_pDocument->GetIndexedLine(m_Y + m_MaxWindowTextLines);
    m_LastScrollDirection = Upward;
  }

  if(pLine == NULL)
  {
    return false;
  }

  m_Y++;

  // Print the new last line
  displayLine(pLine, (m_MaxWindowTextLines - 1) * m_FontHeight);
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
  ScrollRaster(m_pWindow->RPort, 0, -m_FontHeight,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  // Delete the possible visible line below the scroll area which can
  // be caused by the scroll operation above
  EraseRect(m_pWindow->RPort,
    m_ScrollXMin, m_MaxWindowTextLines * m_FontHeight,
    m_ScrollXMax, m_ScrollYMax);

  // Get the line which at current scroll position has to be printed as
  // the windows first line
  SimpleString* pLine = NULL;
  if(m_LastScrollDirection == Downward)
  {
    pLine = m_pDocument->GetPreviousLine();
  }
  else
  {
    pLine = m_pDocument->GetIndexedLine(m_Y - 1);
    m_LastScrollDirection = Downward;
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

struct Image* TextWindow::createImageObj(ULONG p_SysImageId, ULONG& p_Width, ULONG& p_Height)
{
  struct Image* pImage = (struct Image*) NewObject(
      NULL, SYSICLASS,
			SYSIA_Which, p_SysImageId,
			SYSIA_Size, SYSISIZE_MEDRES,
			SYSIA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
			TAG_END);

	if(pImage != NULL)
	{
	  GetAttr(IA_Width, pImage, &p_Width);
	  GetAttr(IA_Height, pImage, &p_Height);
	}

  return pImage;
}
