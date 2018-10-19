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
{

}

DiffWindow::~DiffWindow()
{
  TextWindow::Close();
}

void DiffWindow::Resized()
{
  // Calculate how many lines *now* can be displayed in the window
  calcMaxWindowTextLines();

  if(NumLines() == 0)
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

void DiffWindow::Refresh()
{
  BeginRefresh(m_pWindow);
  displayFile();
  EndRefresh(m_pWindow, TRUE);
}

bool DiffWindow::Open(DW_TYPE p_DwType)
{

  if(p_DwType == RIGHT)
  {
    winLeft = winWidth; // begin one pixel after LEFT window ends
    m_Title = "Right Diff Window";
    activateWin = FALSE;
  }
  
  // TODO Change TextWindow or even Window to use member variables for
  //   WA_Left, WA_Top, WA_Width, WA_Height, winHeight
  // Then set these variables here and call Window::Open()

  // Then remove this:
  
//  //
//  // Opening the window
//  //
//  m_pWindow = OpenWindowTags(NULL,
//    WA_Left, winLeft,
//    WA_Top, screenBarHeight + 1,
//    WA_Width, winWidth,
//    WA_Height, winHeight,
//    WA_Title, (ULONG) m_Title.C_str(),
//    WA_Activate, activateWin,
//    WA_PubScreen, (ULONG) m_pAppScreen->IntuiScreen(),
//    WA_IDCMP, IDCMP_MENUPICK |      // Inform us about menu selection
//              IDCMP_VANILLAKEY |    // Inform us about RAW key press
//              IDCMP_RAWKEY |        // Inform us about printable key press
//              IDCMP_CLOSEWINDOW |   // Inform us about click on close gadget
//              IDCMP_NEWSIZE |       // Inform us about resizing
//              IDCMP_REFRESHWINDOW | // Inform us when refreshing is necessary
//              IDCMP_IDCMPUPDATE,    // Inform us about TODO
//    WA_NewLookMenus, TRUE,          // Ignored before v39
//    WA_Flags, WFLG_CLOSEGADGET |    // Add a close gadget
//              WFLG_DRAGBAR |        // Add a drag gadget
//              WFLG_DEPTHGADGET |    // Add a depth gadget
//              WFLG_SIZEGADGET |     // Add a size gadget
//              WFLG_GIMMEZEROZERO |  // Different layers for border and content
//              WFLG_ACTIVATE,
//    WA_SimpleRefresh, TRUE,
//		WA_MinWidth, 120,
//		WA_MinHeight, 90,
//		WA_MaxWidth, -1,
//		WA_MaxHeight, -1,
//    WA_Gadgets, m_pDownArrowButton,
//    TAG_END);

  return true;
}

virtual bool SetContent(DiffDocument* p_pDiffDocument)
{
  m_pDocument = p_pDiffDocument;
}
