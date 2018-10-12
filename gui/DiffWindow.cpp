#include <string.h>

#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/asl.h>
#include <libraries/dos.h>

#include "DiffWindow.h"

DiffWindow::DiffWindow(AppScreen* p_pAppScreen)
  : m_pAppScreen(p_pAppScreen),
    m_pWindow(NULL),
    m_pWinPropGadgetY(NULL),
    m_MaxWindowTextLines(0),
    m_Y(0),
    m_FontHeight(0),
    m_ScrollXMin(10),
    m_ScrollYMin(10),
    m_ScrollXMax(0),
    m_ScrollYMax(0)
{
  //
  // Calculate some basic values
  //
  m_FontHeight = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
	int titleBarHeight = m_pAppScreen->IntuiScreen()->WBorTop + m_FontHeight + 1; // TODO or +2?

  //
  // Setting up scroll bars and gadgets for the window. They will be
  // attached to the window at opening time
  //
  ULONG sizeGadgetWidth = 18;
  ULONG sizeGadgetHeight = 10;

  // Getting the width of the (currently hidden) size gadget of the window
  struct Image* pSizeImage = (struct Image*) NewObject(
      NULL, SYSICLASS,
			SYSIA_Which, SIZEIMAGE,
			SYSIA_Size, SYSISIZE_MEDRES,
			SYSIA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
			TAG_END);

	if(pSizeImage != NULL)
	{
	  GetAttr(IA_Width, pSizeImage, &sizeGadgetWidth);
	  GetAttr(IA_Width, pSizeImage, &sizeGadgetHeight);
	}

	DisposeObject(pSizeImage);

  //struct Gadget* pWinPropGadgetX;
	m_pWinPropGadgetY = (struct Gadget*) NewObject(
	  NULL, PROPGCLASS,
  	//GA_Previous,uarrowbutton,
  	//GA_ID,GID_YPROP,
  	GA_ID, 916,   // TODO change see above
  	GA_Top, titleBarHeight,
  	GA_Width, sizeGadgetWidth - 6,
  	GA_RelRight, -sizeGadgetWidth + 4,
  	GA_RelHeight, -sizeGadgetHeight - titleBarHeight - 1,
  	GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
  	GA_GZZGadget, TRUE,
  	GA_RightBorder, TRUE,
  	PGA_Freedom, FREEVERT,
  	PGA_Borderless, TRUE,
  	PGA_NewLook, TRUE,
  	PGA_Total, 100,
  	PGA_Top, 0,
  	PGA_Visible, 100,
  	ICA_TARGET, ICTARGET_IDCMP,
  	TAG_END);
}

DiffWindow::~DiffWindow()
{
  Close();
}

void DiffWindow::Resized()
{
  // Calculate how many lines *now* can be displayed in the window
  calcMaxWindowTextLines();

  // Set scroll gadgets pot size dependend on new window size
  if(m_pWinPropGadgetY != NULL)
  {
	  SetGadgetAttrs(m_pWinPropGadgetY, m_pWindow, NULL,
    	PGA_Visible, m_MaxWindowTextLines,
    	TAG_DONE
	   );
  }

}

bool DiffWindow::Open(DW_TYPE p_DwType)
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
  // Calculating window size etc in depency of screen dimesions
  //
  int screenWidth = m_pAppScreen->IntuiScreen()->Width;
  int screenHeight = m_pAppScreen->IntuiScreen()->Height;
  int screenBarHeight = m_pAppScreen->IntuiScreen()->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  int activateWin = TRUE;
  int winLeft = 0;
  m_Title = "Left Diff Window";
  m_FileRequesterTitle = "Select left diff file";

  if(p_DwType == RIGHT)
  {
    winLeft = winWidth; // begin one pixel after LEFT window ends
    m_Title = "Right Diff Window";
    m_FileRequesterTitle = "Select right diff file";
    activateWin = FALSE;
  }

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
              IDCMP_IDCMPUPDATE,    // Inform us about TODO
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Flags, WFLG_CLOSEGADGET |    // Add a close gadget
              WFLG_DRAGBAR |
              WFLG_DEPTHGADGET |
              WFLG_SIZEGADGET |
              WFLG_GIMMEZEROZERO |
              WFLG_ACTIVATE |
              WFLG_NOCAREREFRESH,
    WA_SmartRefresh, TRUE,  // TODO Change to simple refresh??
		WA_MinWidth, 120,
		WA_MinHeight, 90,
		WA_MaxWidth, -1,
		WA_MaxHeight, -1,
    WA_Gadgets, m_pWinPropGadgetY,
    TAG_END);

  // Calculate values needed for text scrolling
  m_ScrollXMax = m_pWindow->Width - m_pWindow->BorderRight - m_ScrollXMin;
  m_ScrollYMax = m_pWindow->Height - m_pWindow->BorderBottom - m_ScrollYMin;

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

void DiffWindow::Close()
{
  if(m_pWinPropGadgetY != NULL)
  {
    DisposeObject(m_pWinPropGadgetY);
  }

  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }
}

const char* DiffWindow::Title()
{
  return m_Title.C_str();
}

void DiffWindow::SetTitle(SimpleString p_NewTitle)
{
  m_Title = p_NewTitle;

  // Call intuition function to set the window title
  // Note the ~0 inverts the value ang is a value of -1
  SetWindowTitles(m_pWindow, m_Title.C_str(), (STRPTR) ~0);
}

struct Window* DiffWindow::IntuiWindow()
{
  return m_pWindow;
}

bool DiffWindow::ReadFile(SimpleString p_FileName)
{
  if(p_FileName.Length() == 0)
  {
    p_FileName = aslRequestFileName();

    if(p_FileName.Length() == 0)
    {
      return false;
    }
  }

  if(DiffDocument::ReadFile(p_FileName) == false)
  {
    return false;
  }

  // Set full path of opened file as window title
  SetTitle(p_FileName);
  displayFile();

  // Set scroll gadgets pot size dependend on window size and the number
  // of lines in opened file
  if(m_pWinPropGadgetY != NULL)
  {
	  SetGadgetAttrs(m_pWinPropGadgetY, m_pWindow, NULL,
    	PGA_Total, NumLines(),
    	PGA_Top, 0,
    	PGA_Visible, m_MaxWindowTextLines,
    	TAG_DONE
	   );
  }

  return true;
}

void DiffWindow::ScrollUpOneLine()
{
  if(NumLines() < m_MaxWindowTextLines)
  {
    // Do not move down if all the text fits into the window
    return;
  }

  if(m_Y == NumLines() - 1)
  {
    // Do not move the text up if already at top
    return;
  }

  // Scroll upward one line by the current font height
  ScrollRaster(m_pWindow->RPort, 0, m_FontHeight,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  m_Y++;

  SimpleString* pLine = GetIndexedLine(m_Y);
  displayLine(pLine, (m_MaxWindowTextLines - 1) * m_FontHeight);
}

void DiffWindow::ScrollDownOneLine()
{
  if(m_Y < m_MaxWindowTextLines)
  {
    // Do not move the text down if already at bottom
    return;
  }

  // Scroll upward one line by the current font height
  ScrollRaster(m_pWindow->RPort, 0, -m_FontHeight,
    m_ScrollXMin, m_ScrollYMin, m_ScrollXMax, m_ScrollYMax);

  m_Y--;

  SimpleString* pLine = GetIndexedLine(m_Y - m_MaxWindowTextLines + 1);
  displayLine(pLine, 0);


}

void DiffWindow::calcMaxWindowTextLines()
{
  m_MaxWindowTextLines = m_pWindow->Height;
  m_MaxWindowTextLines -= m_pWindow->BorderTop;
  m_MaxWindowTextLines -= m_pWindow->BorderBottom;
  m_MaxWindowTextLines -= m_ScrollYMin;
  m_MaxWindowTextLines /= m_FontHeight;
}

SimpleString DiffWindow::aslRequestFileName()
{
  SimpleString fileName = "";

  struct TagItem fileRequestTags[] =
  {
    ASL_Hail, (ULONG)m_FileRequesterTitle.C_str(),
    TAG_DONE
  };

  struct TagItem requestOpeningTags[] =
  {
    ASLFR_Window, (ULONG)m_pWindow,
    TAG_DONE
  };

  struct FileRequester* pFileRequest;
  pFileRequest = (struct FileRequester*)
    AllocAslRequest(ASL_FileRequest, fileRequestTags);

  if(pFileRequest == NULL)
  {
    return fileName;
  }

  if(AslRequest(pFileRequest,requestOpeningTags))
  {
    // Copying path name into a big enough buffer
    char fullPathBuf[512];
    strcpy(fullPathBuf, pFileRequest->rf_Dir);

    // Calling a dos.library function to combine path and file name
    if(AddPart(fullPathBuf, pFileRequest->fr_File, 512))
    {
     fileName = fullPathBuf;
    }
  }

  FreeAslRequest(pFileRequest);

  return fileName;
}

void DiffWindow::displayLine(SimpleString* p_pLine, WORD p_TopEdge)
{
  m_IntuiText.IText = (UBYTE*)p_pLine->C_str();
  m_IntuiText.TopEdge = p_TopEdge;
  PrintIText(m_pWindow->RPort, &m_IntuiText, m_ScrollXMin, m_ScrollYMin);
}

void DiffWindow::displayFile()
{
  SimpleString* pLine = GetFirstLine();
  m_Y = 0;
  while(pLine != NULL)
  {
    displayLine(pLine, m_Y * m_FontHeight);

    if(m_Y >= m_MaxWindowTextLines - 1)
    {
      break;
    }

    m_Y++;
    pLine = GetNextLine();
  }
}
