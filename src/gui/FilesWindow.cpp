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
    m_pGadgetListHead(NULL),
    m_pGadgetLeftFile(NULL),
    m_pGadgetRightFile(NULL),
    m_pGadgetSelectLeft(NULL),
    m_pGadgetSelectRight(NULL),
    m_pGadgetDiff(NULL),
    m_pGadgetSwap(NULL),
    m_pGadgetCancel(NULL)
{

}


FilesWindow::~FilesWindow()
{
  Close();

  if(m_pGadgetListHead != NULL)
  {
    FreeGadgets(m_pGadgetListHead);
    m_pGadgetListHead = NULL;
    m_pGadgetLeftFile = NULL;
    m_pGadgetRightFile = NULL;
    m_pGadgetSelectLeft = NULL;
    m_pGadgetSelectRight = NULL;
    m_pGadgetDiff = NULL;
    m_pGadgetSwap = NULL;
    m_pGadgetCancel = NULL;
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

  // Set the Diff button to an initial enabled / disabled state
  setDiffButtonState();

  setStringGadgetText(m_pGadgetLeftFile, m_LeftFilePath);
  setStringGadgetText(m_pGadgetRightFile, m_RightFilePath);

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
      if(pGadget->GadgetID == GID_LeftFileButton)
      {
        // Button "..." to select left file was clicked
        return selectLeftFile();
      }
      else if(pGadget->GadgetID == GID_RightFileButton)
      {
        // Button "..." to select right file was clicked
        return selectRightFile();
      }
      else if(pGadget->GadgetID == GID_CancelButton)
      {
        // Button "Cancel" was clicked
        Close();
      }
      else if(pGadget->GadgetID == GID_DiffButton)
      {
        // Button "Diff" was clicked

        // Read latest string gadgets contents before continue
        readStringGadgetsText();

        // If now one of the texts is empty, do not perform the Diff
        if((m_LeftFilePath.Length()) == 0 ||
           (m_RightFilePath.Length()) == 0)
        {
          // Note: true marks that the *idcmp* was handled properly,
          //       regardless of the diff not being performed
          return true;
        }

        // Perform the diff
        m_CmdDiff.Execute();
      }
      else if(pGadget->GadgetID == GID_LeftFileString)
      {
        // Text in left file string gadget was changed
        readStringGadgetsText();
      }
      else if(pGadget->GadgetID == GID_RightFileString)
      {
        // Text in right file string gadget was changed
        readStringGadgetsText();
      }
      return true;
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
      GT_BeginRefresh(IntuiWindow());
      GT_EndRefresh(IntuiWindow(), TRUE);
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
  }

  return false;
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
  WORD buttonWidth = 60;
  WORD buttonHeight = m_FontHeight + 6;
  WORD selectButtonWidth = TextLength(
    &m_AppScreen.IntuiScreen()->RastPort, "...", 3) + 8;
  WORD stringGadgetWidth = right - left - hSpace / 2 - selectButtonWidth;
  WORD selectButtonLeft = right - selectButtonWidth;

  //
  // Setting up the gadgets
  //

  // Create a place for GadTools context data
  struct Gadget* pContext;
  pContext = (struct Gadget*) CreateContext(&m_pGadgetListHead);
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
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Left file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  struct Gadget* pLabelGadget = CreateGadget(TEXT_KIND,
    pContext, &newGadget, TAG_END);

  // Row 2 contains a string gadget and selection button for the
  // filename of the left file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pGadgetLeftFile = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget,
    GTST_MaxChars, 200, // TODO remove constant
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;
  newGadget.ng_Flags      = 0;

  m_pGadgetSelectLeft = CreateGadget(BUTTON_KIND,
    m_pGadgetLeftFile, &newGadget, TAG_END);

  // Row 3  contains a label
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  pLabelGadget = CreateGadget(TEXT_KIND, m_pGadgetSelectLeft,
    &newGadget, TAG_END);

  // Row 4 contains a string gadget and selection button for the
  // filename of the right file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_RightFileString;
  newGadget.ng_Flags      = 0;

  m_pGadgetRightFile = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget,
    GTST_MaxChars, 200, // TODO remove constant
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;
  newGadget.ng_Flags      = 0;

  m_pGadgetSelectRight = CreateGadget(BUTTON_KIND,
    m_pGadgetRightFile, &newGadget, TAG_END);

  // Row 5 conatins the buttons Diff and Cancel

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace + vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pGadgetDiff = CreateGadget(BUTTON_KIND,
    m_pGadgetSelectRight, &newGadget, GT_Underscore, '_', TAG_END);

  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pGadgetCancel = CreateGadget(BUTTON_KIND,
    m_pGadgetDiff, &newGadget, GT_Underscore, '_', TAG_END);

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
           IDCMP_VANILLAKEY |     // Inform about RAW key press
           IDCMP_RAWKEY |         // Inform about printable key press
           IDCMP_CLOSEWINDOW |    // Inform about click on close gadget
           IDCMP_REFRESHWINDOW |  // Inform when refreshing is necessary
           BUTTONIDCMP);          // Inform about GadTools button events

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadgetListHead);

  m_bInitialized = true;
}


bool FilesWindow::selectLeftFile()
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
    bPathOnly = true;
  }

  SimpleString title("Select left (original) file");
  SimpleString sel = m_AslRequest.SelectFile(title,
                                             path,
                                             bPathOnly);

  if(sel.Length() == 0)
  {
    enableAll();

    // Exit with success because idcmp was properly handled
    return true;
  }

  m_LeftFilePath = sel;
  setStringGadgetText(m_pGadgetLeftFile, m_LeftFilePath);

  enableAll();
  return true;
}


bool FilesWindow::selectRightFile()
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
    bPathOnly = true;
  }

  SimpleString title("Select right (changed) file");
  SimpleString sel = m_AslRequest.SelectFile(title, path, bPathOnly);

  if(sel.Length() == 0)
  {
    enableAll();

    // Exit with success because idcmp was properly handled
    return true;
  }

  m_RightFilePath = sel;
  setStringGadgetText(m_pGadgetRightFile, m_RightFilePath);

  enableAll();
  return true;
}


void FilesWindow::enableAll()
{
  GT_SetGadgetAttrs(m_pGadgetLeftFile, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetRightFile, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetSelectLeft, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetSelectRight, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetCancel, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  setDiffButtonState();

  // TODO enable menu item "Quit"
}

void FilesWindow::disableAll()
{
  GT_SetGadgetAttrs(m_pGadgetLeftFile, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetRightFile, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetSelectLeft, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetSelectRight, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetCancel, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pGadgetDiff, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  // TODO disable menu item "Quit"
}


void FilesWindow::setDiffButtonState()
{
  if(!IsOpen() || m_pGadgetDiff == NULL)
  {
    return;
  }

  if(m_LeftFilePath.Length() > 0 &&
     m_RightFilePath.Length() > 0)
  {
    // Enable "Diff" button
    GT_SetGadgetAttrs(m_pGadgetDiff, m_pWindow, NULL,
      GA_Disabled, FALSE,
      TAG_END);
  }
  else
  {
    // Disable "Diff" button
    GT_SetGadgetAttrs(m_pGadgetDiff, m_pWindow, NULL,
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
    m_pGadgetLeftFile->SpecialInfo)->Buffer;

  m_LeftFilePath = (const char*)pBuf;

  // Set the changed string gadget text as right file path
  pBuf = ((struct StringInfo*)
    m_pGadgetRightFile->SpecialInfo)->Buffer;

  m_RightFilePath = (const char*)pBuf;

  // Enable the 'Diff' button when both string gadgets contain text.
  // If not not: disable it.
  setDiffButtonState();
}

