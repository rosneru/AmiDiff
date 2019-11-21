#include <string.h>
#include <stdio.h>

#include <clib/dos_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>

#include "FilesWindow.h"

FilesWindow::FilesWindow(AppScreen& appScreen,
                         struct MsgPort* pMsgPort,
                         int& numWindowsOpen,
                         SimpleString& leftFilePath,
                         SimpleString& rightFilePath,
                         const Command& cmdDiff)
  : WindowBase(appScreen, pMsgPort, numWindowsOpen),
    m_AslRequest(m_pWindow),
    m_bFileRequestOpen(false),
    m_LeftFilePath(leftFilePath),
    m_RightFilePath(rightFilePath),
    m_CmdDiff(cmdDiff),
    m_pGadgetsHeader(NULL),
    m_pGadStrLeftFile(NULL),
    m_pGadStrRightFile(NULL),
    m_pGadBtnSelectLeft(NULL),
    m_pGadBtnSelectRight(NULL),
    m_pGadBtnDiff(NULL),
    m_pGadBtnSwap(NULL),
    m_pGadBtnCancel(NULL)
{

}


FilesWindow::~FilesWindow()
{
  Close();

  if(m_pGadgetsHeader != NULL)
  {
    FreeGadgets(m_pGadgetsHeader);
    m_pGadgetsHeader = NULL;
    m_pGadStrLeftFile = NULL;
    m_pGadStrRightFile = NULL;
    m_pGadBtnSelectLeft = NULL;
    m_pGadBtnSelectRight = NULL;
    m_pGadBtnDiff = NULL;
    m_pGadBtnSwap = NULL;
    m_pGadBtnCancel = NULL;
  }
}


void FilesWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool FilesWindow::Open(const APTR pMenuItemDisableAtOpen)
{
  if(WindowBase::Open(pMenuItemDisableAtOpen) == false)
  {
    return false;
  }

  setStringGadgetText(m_pGadStrLeftFile, m_LeftFilePath);
  setStringGadgetText(m_pGadStrRightFile, m_RightFilePath);

  // Enable or disable the 'Diff' and 'Swap' buttons depending on some
  // conditions
  enableIfPossible();


  return true;
}


bool FilesWindow::HandleIdcmp(ULONG msgClass,
                              UWORD msgCode,
                              APTR pItemAddress)
{
  if(!IsOpen())
  {
    return false;
  }

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
      handleGadgetEvent(pGadget);
      return true;
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
      GT_BeginRefresh(m_pWindow);
      GT_EndRefresh(m_pWindow, TRUE);
      return true;
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      if(!m_bFileRequestOpen)
      {
        Close();
      }
      return true;
      break;
    }

    case IDCMP_VANILLAKEY:
    {
      handleVanillaKey(msgCode);
      return true;
      break;
    }
  }

  return false;
}


void FilesWindow::handleGadgetEvent(struct Gadget* pGadget)
{
  if(pGadget == NULL)
  {
    return;
  }

  switch(pGadget->GadgetID)
  {
    case GID_LeftFileString:
    case GID_RightFileString:
      readStringGadgetsText();
      break;

    case GID_LeftFileButton:  // Select left file
      selectLeftFile();
      break;

    case GID_RightFileButton: // Select right file
      selectRightFile();
      break;

    case GID_SwapButton:      // Swap left and right file
      swapFiles();
      break;

    case GID_DiffButton:      // Compare the files and display the diff
      compare();
      break;

    case GID_CancelButton:
      Close();
      break;
  }
}


void FilesWindow::handleVanillaKey(UWORD code)
{
  switch(code)
  {
    case 'l': // Select left file
    case 'L':
      selectLeftFile();
      break;

    case 'r': // Select right file
    case 'R':
      selectRightFile();
      break;

    case 's': // Swap left and right file
    case 'S':
      // TODO Allow only if Swap button is enabled
      //GT_GetGadgetAttrs
      swapFiles();
      break;

    case 'd': // Compare the files and display the diff
    case 'D':
    {
      // TODO Allow only if Diff button is enabled
      //GT_GetGadgetAttrs
      compare();
      break;
    }


    case 'c': // Cancel
    case 'C':
      Close();
      break;

  }
}


void FilesWindow::initialize()
{
  //
  // Calculate some basic values
  //
  m_WinWidth = (WORD)m_AppScreen.IntuiScreen()->Width / 2;
  m_FontHeight = m_AppScreen.IntuiDrawInfo()->dri_Font->tf_YSize;
  WORD barHeight = m_AppScreen.IntuiScreen()->WBorTop + m_FontHeight + 2;

  WORD hSpace = 10;
  WORD vSpace = 10;

  WORD top = barHeight + vSpace;
  WORD left = hSpace;
  WORD right = m_WinWidth - hSpace;

  WORD btnsWidth = 60;
  WORD btnsHeight = m_FontHeight + 6;

  WORD btnSelectWidth = TextLength(&m_AppScreen.IntuiScreen()->RastPort, "...", 3) + 8;
  WORD btnSelectLeft = right - btnSelectWidth;

  WORD stringGadWidth = right - left - hSpace / 2 - btnSelectWidth;

  //
  // Setting up the gadgets
  //

  // Create a place for GadTools context data
  struct Gadget* pContext;
  pContext = (struct Gadget*) CreateContext(&m_pGadgetsHeader);
  if(pContext == NULL)
  {
    return;
  }

  // Declare the basic gadget structure
  struct NewGadget newGadget;

  // Row 1  contains  a label
  newGadget.ng_TextAttr   = m_AppScreen.GfxTextAttr();
  newGadget.ng_VisualInfo = m_AppScreen.GadtoolsVisualInfo();
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = stringGadWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "_Left file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  struct Gadget* pLabelGadget = CreateGadget(TEXT_KIND,
                                             pContext,
                                             &newGadget,
                                             GT_Underscore, '_',
                                             TAG_END);

  // Row 2 contains a string gadget and selection button for the
  // filename of the left file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadWidth;
  newGadget.ng_Height     = btnsHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pGadStrLeftFile = CreateGadget(STRING_KIND,
                                   pLabelGadget,
                                   &newGadget,
                                   GTST_MaxChars, 200, // TODO remove constant
                                   TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = btnSelectLeft;
  newGadget.ng_Width      = btnSelectWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;
  newGadget.ng_Flags      = 0;

  m_pGadBtnSelectLeft = CreateGadget(BUTTON_KIND,
                                     m_pGadStrLeftFile,
                                     &newGadget,
                                     TAG_END);

  // Row 3  contains a label
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    += btnsHeight + vSpace;
  newGadget.ng_Width      = stringGadWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "_Right file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  pLabelGadget = CreateGadget(TEXT_KIND,
                              m_pGadBtnSelectLeft,
                              &newGadget,
                              GT_Underscore, '_',
                              TAG_END);

  // Row 4 contains a string gadget and selection button for the
  // filename of the right file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadWidth;
  newGadget.ng_Height     = btnsHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_RightFileString;
  newGadget.ng_Flags      = 0;

  m_pGadStrRightFile = CreateGadget(STRING_KIND,
                                    pLabelGadget,
                                    &newGadget,
                                    GTST_MaxChars, 200, // TODO remove constant
                                    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = btnSelectLeft;
  newGadget.ng_Width      = btnSelectWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;
  newGadget.ng_Flags      = 0;

  m_pGadBtnSelectRight = CreateGadget(BUTTON_KIND,
                                      m_pGadStrRightFile,
                                      &newGadget,
                                      TAG_END);

  // Row 5 conatins the buttons Diff, Swap and Cancel

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += btnsHeight + vSpace + vSpace;
  newGadget.ng_Width      = btnsWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pGadBtnDiff = CreateGadget(BUTTON_KIND,
                               m_pGadBtnSelectRight,
                               &newGadget,
                               GT_Underscore, '_',
                               TAG_END);

  // Creating the Swap button
  newGadget.ng_LeftEdge   = (right - left - btnsWidth) / 2;
  newGadget.ng_GadgetText = (UBYTE*) "_Swap";
  newGadget.ng_GadgetID   = GID_SwapButton;

  m_pGadBtnSwap = CreateGadget(BUTTON_KIND,
                               m_pGadBtnDiff,
                               &newGadget,
                               GT_Underscore, '_',
                               TAG_END);


  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - btnsWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pGadBtnCancel = CreateGadget(BUTTON_KIND,
                                 m_pGadBtnSwap,
                                 &newGadget,
                                 GT_Underscore, '_',
                                 TAG_END);

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_WinHeight = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  SetTitle("Open the files to diff");

  // Setting the window flags
  setFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DRAGBAR |         // Add a drag gadget
           WFLG_DEPTHGADGET);     // Add a depth gadget

  // Setting the IDCMP messages we want to receive for this window
  setIDCMP(IDCMP_MENUPICK |       // Inform about menu selection
           IDCMP_VANILLAKEY |     // Inform about key press
           IDCMP_CLOSEWINDOW |    // Inform about click on close gadget
           IDCMP_REFRESHWINDOW |  // Inform when refreshing is necessary
           BUTTONIDCMP);          // Inform about Gadget events

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadgetsHeader);

  m_bInitialized = true;
}


void FilesWindow::selectLeftFile()
{
  disableAll();

  // Read latest string gadgets contents before continue
  readStringGadgetsText();

  SimpleString path = m_LeftFilePath;
  bool bPathOnly = false;
  if((m_LeftFilePath.Length() == 0) &&
     (m_RightFilePath.Length() > 0))
  {
    // Left file path is empty, so use the path of the right file for
    // pre-selection
    path = m_RightFilePath;

    // Do not use the file name 'though
    bPathOnly = true;
  }

  SimpleString title("Select left (original) file");
  SimpleString sel = m_AslRequest.SelectFile(title,
                                             path,
                                             bPathOnly);

  if(sel.Length() == 0)
  {
    enableAll();
    return;
  }

  m_LeftFilePath = sel;
  setStringGadgetText(m_pGadStrLeftFile, m_LeftFilePath);

  enableAll();
  return;
}


void FilesWindow::selectRightFile()
{
  disableAll();

  // Read latest string gadgets contents before continue
  readStringGadgetsText();

  SimpleString path = m_RightFilePath;
  bool bPathOnly = false;
  if((m_RightFilePath.Length() == 0) &&
     (m_LeftFilePath.Length() > 0))
  {
    // Right file path is empty, so use the path of the left file for
    // pre-selection
    path = m_LeftFilePath;

    // Do not use the file name 'though
    bPathOnly = true;
  }

  SimpleString title("Select right (changed) file");
  SimpleString sel = m_AslRequest.SelectFile(title, path, bPathOnly);

  if(sel.Length() == 0)
  {
    enableAll();
    return;
  }

  m_RightFilePath = sel;
  setStringGadgetText(m_pGadStrRightFile, m_RightFilePath);

  enableAll();
  return;
}


void FilesWindow::swapFiles()
{
  // TODO
}


void FilesWindow::compare()
{
  // Read latest string gadgets contents before continue
  readStringGadgetsText();

  // If now one of the texts is empty, do not perform the Diff
  if((m_LeftFilePath.Length()) == 0 ||
     (m_RightFilePath.Length()) == 0)
  {
    return;
  }

  // Perform the diff
  m_CmdDiff.Execute();

}


void FilesWindow::enableAll()
{
  GT_SetGadgetAttrs(m_pGadStrLeftFile, m_pWindow, NULL,
                    GA_Disabled, FALSE,
                    TAG_END);

  GT_SetGadgetAttrs(m_pGadStrRightFile, m_pWindow, NULL,
                    GA_Disabled, FALSE,
                    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnSelectLeft, m_pWindow, NULL,
                    GA_Disabled, FALSE,
                    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnSelectRight, m_pWindow, NULL,
                    GA_Disabled, FALSE,
                    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnCancel, m_pWindow, NULL,
                    GA_Disabled, FALSE,
                    TAG_END);

  // Enable or disable the 'Diff' and 'Swap' buttons depending on some
  // conditions
  enableIfPossible();

  // TODO enable menu item "Quit"
}

void FilesWindow::disableAll()
{
  GT_SetGadgetAttrs(m_pGadStrLeftFile, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadStrRightFile, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnSelectLeft, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnSelectRight, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnCancel, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnSwap, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadBtnDiff, m_pWindow, NULL,
    GA_Disabled, TRUE,
    TAG_END);

  // TODO disable menu item "Quit"
}


void FilesWindow::enableIfPossible()
{
  if(!IsOpen() || (m_pGadBtnDiff == NULL) || (m_pGadBtnSwap == NULL))
  {
    return;
  }

  if((m_LeftFilePath.Length()) > 0
   &&(m_RightFilePath.Length()) > 0)
  {
    // Enable "Diff" button
    GT_SetGadgetAttrs(m_pGadBtnDiff, m_pWindow, NULL,
                      GA_Disabled, FALSE,
                      TAG_END);
  }
  else
  {
    // Disable "Diff" button
    GT_SetGadgetAttrs(m_pGadBtnDiff, m_pWindow, NULL,
                      GA_Disabled, TRUE,
                      TAG_END);
  }

  if((m_LeftFilePath.Length()) > 0
   ||(m_RightFilePath.Length()) > 0)
  {
    // Enable "Swap" button
    GT_SetGadgetAttrs(m_pGadBtnSwap, m_pWindow, NULL,
                      GA_Disabled, FALSE,
                      TAG_END);
  }
  else
  {
    // Disable "Swap" button
    GT_SetGadgetAttrs(m_pGadBtnSwap, m_pWindow, NULL,
                      GA_Disabled, TRUE,
                      TAG_END);
  }


}

void FilesWindow::setStringGadgetText(struct Gadget* pGadget,
  const SimpleString& text)
{
  if(!IsOpen() || pGadget == NULL)
  {
    return;
  }

  GT_SetGadgetAttrs(pGadget, m_pWindow, NULL,
    GTST_String, text.C_str(),
    TAG_END);
}

void FilesWindow::readStringGadgetsText()
{
  // Set the changed string gadget text as left file path
  UBYTE* pBuf = ((struct StringInfo*)
    m_pGadStrLeftFile->SpecialInfo)->Buffer;

  m_LeftFilePath = (const char*)pBuf;

  // Set the changed string gadget text as right file path
  pBuf = ((struct StringInfo*)
    m_pGadStrRightFile->SpecialInfo)->Buffer;

  m_RightFilePath = (const char*)pBuf;

  // Enable or disable the 'Diff' and 'Swap' buttons depending on some
  // conditions
  enableIfPossible();
}

