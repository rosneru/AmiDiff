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
#include "OpenFilesWindow.h"

OpenFilesWindow::OpenFilesWindow(AppScreen* p_pAppScreen)
  : WindowBase(p_pAppScreen),
    m_WinWidth(240),
    m_WinHeight(180),
    m_pLeftFileStringGadget(NULL),
    m_pRightFileStringGadget(NULL),
    m_pOpenLeftFileButton(NULL),
    m_pOpenRightFileButton(NULL),
    m_pDiffButton(NULL),
    m_pCancelButton(NULL)
{
  //
  // Calculate some basic values
  //
  m_FontHeight = m_pAppScreen->IntuiDrawInfo()->dri_Font->tf_YSize;
	int barHeight = m_pAppScreen->IntuiScreen()->WBorTop + m_FontHeight + 2;

  WORD hSpace = 5;
  WORD vSpace = 3;
  WORD selectButtonWidth = m_FontHeight + 2; // shoud be square
  WORD stringGadgetWidth = m_WinWidth - hSpace - hSpace - hSpace - selectButtonWidth;
  WORD selectButtonLeft = hSpace + stringGadgetWidth + hSpace;
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + 2;
  m_WinHeight = vSpace + vSpace + 3 * (buttonHeight + vSpace);

  // Creating the string gadget for the file name of the left file
  m_pLeftFileStringGadget = (struct Gadget*) NewObject(
    NULL, STRGCLASS,
    GA_ID, OFW_LEFT_FILE_STRING,
    GA_LEFT, hSpace,
    GA_TOP, vSpace,
    GA_WIDTH, stringGadgetWidth,
    GA_HEIGHT, buttonHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_BottomBorder, TRUE,
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);
    
  // Button for opening a file selector for left file
  m_pOpenLeftFileButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pLeftFileStringGadget,
    GA_ID, OFW_LEFT_FILE_BUTTON,
    GA_LEFT, selectButtonLeft,
    GA_TOP, vSpace,
    GA_WIDTH, selectButtonWidth,
    GA_HEIGHT, buttonHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_BottomBorder, TRUE,
    GA_Text, (const char*) "...",
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the string gadget for the file name of the right file
	m_pRightFileStringGadget = (struct Gadget*) NewObject(
	  NULL, STRGCLASS,
  	GA_Previous, m_pOpenLeftFileButton,
  	GA_ID, OFW_RIGHT_FILE_STRING,
    GA_LEFT, hSpace,
    GA_TOP, vSpace + vSpace + buttonHeight,
    GA_WIDTH, stringGadgetWidth,
    GA_HEIGHT, buttonHeight,
  	GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
  	GA_RightBorder, TRUE,
  	ICA_TARGET, ICTARGET_IDCMP,
  	TAG_END);

  // Button for opening a file selector for right file
  m_pOpenRightFileButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pRightFileStringGadget,
    GA_ID, OFW_RIGHT_FILE_BUTTON,
    GA_LEFT, selectButtonLeft,
    GA_TOP, vSpace + vSpace + buttonHeight,
    GA_WIDTH, selectButtonWidth,
    GA_HEIGHT, buttonHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_BottomBorder, TRUE,
    GA_Text, (const char*) "...",
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the Diff button
  m_pDiffButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pOpenRightFileButton,
    GA_ID, OFW_DIFF_BUTTON,
    GA_LEFT, hSpace,
    GA_RELBOTTOM, - buttonHeight,
    GA_WIDTH, buttonWidth,
    GA_HEIGHT, buttonHeight,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_RightBorder, TRUE,
    GA_Text, (const char*) "Diff",
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);

  // Creating the Cancel button
  m_pCancelButton = (struct Gadget*) NewObject(
    NULL, BUTTONGCLASS,
    GA_Previous, m_pDiffButton,
    GA_ID, OFW_CANCEL_BUTTON,
    GA_RELRIGHT, -buttonWidth-hSpace,
    GA_RELBOTTOM, -buttonHeight,
    GA_WIDTH, buttonWidth,
    GA_HEIGHT, m_FontHeight + 4,
    GA_DrawInfo, m_pAppScreen->IntuiDrawInfo(),
    GA_RightBorder, TRUE,
    GA_Text, (const char*) "Cancel",
    ICA_TARGET, ICTARGET_IDCMP,
    TAG_END);
}

OpenFilesWindow::~OpenFilesWindow()
{
  Close();
}





void OpenFilesWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool OpenFilesWindow::Open()
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

  int winLeft = screenWidth / 2 - m_WinWidth / 2;
  int winTop = screenHeight / 2 - m_WinHeight / 2;

  //
  // Opening the window
  //
  m_pWindow = OpenWindowTags(NULL,
    WA_Left, winLeft,
    WA_Top, winTop,
    WA_Width, m_WinWidth,
    WA_Height, m_WinHeight,
    WA_Title, (ULONG) "Open the files to diff",
    WA_Activate, TRUE,
    WA_PubScreen, (ULONG) m_pAppScreen->IntuiScreen(),
    WA_IDCMP, IDCMP_REFRESHWINDOW | // Inform us when refreshing is necessary
              IDCMP_IDCMPUPDATE,    // Inform us about BOOPSI Gadget updates
    WA_Flags, WFLG_DRAGBAR |        // Add a drag gadget
              WFLG_DEPTHGADGET |    // Add a depth gadget
              WFLG_ACTIVATE,
    WA_SimpleRefresh, TRUE,
    WA_Gadgets, m_pLeftFileStringGadget,
    TAG_END);


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

void OpenFilesWindow::Close()
{
  if(m_pOpenLeftFileButton != NULL)
  {
    DisposeObject(m_pOpenLeftFileButton);
  }

  if(m_pOpenRightFileButton != NULL)
  {
    DisposeObject(m_pOpenRightFileButton);
  }

  if(m_pLeftFileStringGadget != NULL)
  {
    DisposeObject(m_pLeftFileStringGadget);
  }

  if(m_pRightFileStringGadget != NULL)
  {
    DisposeObject(m_pRightFileStringGadget);
  }

  if(m_pCancelButton != NULL)
  {
    DisposeObject(m_pCancelButton);
  }

  if(m_pDiffButton != NULL)
  {
    DisposeObject(m_pDiffButton);
  }

  // Also call Close() in parent
  // TODO debug if it really happens
  WindowBase::Close();
}


void OpenFilesWindow::HandleIdcmp(struct IntuiMessage* p_pMsg)
{
  switch (p_pMsg->Class)
  {
    case IDCMP_IDCMPUPDATE:
    {
      ULONG tagData = GetTagData(GA_ID, 0,
        (struct TagItem *)p_pMsg->IAddress);
      switch(tagData)
      {
        case OpenFilesWindow::OFW_LEFT_FILE_BUTTON:
        {
          size_t newY = GetTagData(PGA_Top, 0, (struct TagItem *)
            p_pMsg->IAddress);

          // YChangedHandler(newY);
          break;
        }

        case OpenFilesWindow::OFW_RIGHT_FILE_BUTTON:
        {
          // YDecrease();
          break;
        }

        case OpenFilesWindow::OFW_DIFF_BUTTON:
        {
          // YIncrease();
          break;
        }

        case OpenFilesWindow::OFW_CANCEL_BUTTON:
        {
          // YIncrease();
          break;
        }

      }
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      Refresh();
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      //m_pCmdQuit->Execute(); TODO
      break;
    }
  }
}
