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

#include "AslFileRequest.h"
#include "FilesWindow.h"

FilesWindow::FilesWindow(AppScreen& p_AppScreen,
                         struct MsgPort* p_pMsgPort, 
                         int& p_NumWindowsOpen,
                         SimpleString& p_LeftFilePath,
                         SimpleString& p_RightFilePath,
                         const Command& p_CmdDiff)
  : WindowBase(p_AppScreen, p_pMsgPort, p_NumWindowsOpen),
    m_bFileRequestOpen(false),
    m_LeftFilePath(p_LeftFilePath),
    m_RightFilePath(p_RightFilePath),
    m_CmdDiff(p_CmdDiff),
    m_pGadgetList(NULL),
    m_pLeftFileStringGadget(NULL),
    m_pRightFileStringGadget(NULL),
    m_pSelectLeftFileButton(NULL),
    m_pSelectRightFileButton(NULL),
    m_pDiffButton(NULL),
    m_pCancelButton(NULL)
{

}

FilesWindow::~FilesWindow()
{
  Close();

  if(m_pGadgetList != NULL)
  {
    FreeGadgets(m_pGadgetList);
    m_pGadgetList = NULL;
    m_pLeftFileStringGadget = NULL;
    m_pRightFileStringGadget = NULL;
    m_pSelectLeftFileButton = NULL;
    m_pSelectRightFileButton = NULL;
    m_pDiffButton = NULL;
    m_pCancelButton = NULL;
  }
}





void FilesWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool FilesWindow::Open(const APTR p_pUserDataMenuItemToDisable)
{
  if(WindowBase::Open(p_pUserDataMenuItemToDisable) == false)
  {
    return false;
  }

  // Set the Diff button to an initial enabled / disabled state
  setDiffButtonState();

  setStringGadgetText(m_pLeftFileStringGadget, m_LeftFilePath);
  setStringGadgetText(m_pRightFileStringGadget, m_RightFilePath);

  return true;
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
  pContext = (struct Gadget*) CreateContext(&m_pGadgetList);
  if(pContext == NULL)
  {
    return;
  }

  // Declare the basic gadget structure
  struct NewGadget newGadget;

  // Line 1  contains  a label
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

  // Line 2 contains a string gadget and selection button for the
  // filename of the left file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_LeftFileString;
  newGadget.ng_Flags      = 0;

  m_pLeftFileStringGadget = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget,
    GTST_MaxChars, 200, // TODO remove constant
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_LeftFileButton;
  newGadget.ng_Flags      = 0;

  m_pSelectLeftFileButton = CreateGadget(BUTTON_KIND,
    m_pLeftFileStringGadget, &newGadget, TAG_END);

  // Line 3  contains a label
  newGadget.ng_LeftEdge   = left + 2;
  newGadget.ng_TopEdge    += buttonHeight + vSpace;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = m_FontHeight;
  newGadget.ng_GadgetText = (UBYTE*) "Right file";
  newGadget.ng_Flags = PLACETEXT_RIGHT | PLACETEXT_LEFT | NG_HIGHLABEL;

  pLabelGadget = CreateGadget(TEXT_KIND, m_pSelectLeftFileButton,
    &newGadget, TAG_END);

  // Line 4 contains a string gadget and selection button for the
  // filename of the right file

  // Creating the string gadget
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += m_FontHeight + 2;
  newGadget.ng_Width      = stringGadgetWidth;
  newGadget.ng_Height     = buttonHeight;
  newGadget.ng_GadgetText = NULL;
  newGadget.ng_GadgetID   = GID_RightFileString;
  newGadget.ng_Flags      = 0;

  m_pRightFileStringGadget = CreateGadget(STRING_KIND,
    pLabelGadget, &newGadget,
    GTST_MaxChars, 200, // TODO remove constant
    TAG_END);

  // Creating the Select button
  newGadget.ng_LeftEdge   = selectButtonLeft;
  newGadget.ng_Width      = selectButtonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "...";
  newGadget.ng_GadgetID   = GID_RightFileButton;
  newGadget.ng_Flags      = 0;

  m_pSelectRightFileButton = CreateGadget(BUTTON_KIND,
    m_pRightFileStringGadget, &newGadget, TAG_END);

  // Line 5 conatins the buttons Diff and Cancel

  // Creating the Diff button
  newGadget.ng_LeftEdge   = left;
  newGadget.ng_TopEdge    += buttonHeight + vSpace + vSpace;
  newGadget.ng_Width      = buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Diff";
  newGadget.ng_GadgetID   = GID_DiffButton;

  m_pDiffButton = CreateGadget(BUTTON_KIND,
    m_pSelectRightFileButton, &newGadget, GT_Underscore, '_', TAG_END);

  // Creating the Cancel button
  newGadget.ng_LeftEdge   = right - buttonWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Cancel";
  newGadget.ng_GadgetID   = GID_CancelButton;

  m_pCancelButton = CreateGadget(BUTTON_KIND,
    m_pDiffButton, &newGadget, GT_Underscore, '_', TAG_END);

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
  setIDCMP(IDCMP_MENUPICK |       // Inform us about menu selection
           IDCMP_VANILLAKEY |     // Inform us about RAW key press
           IDCMP_RAWKEY |         // Inform us about printable key press
           IDCMP_CLOSEWINDOW |    // Inform us about click on close gadget
           IDCMP_REFRESHWINDOW |  // Inform us when refreshing is necessary
           BUTTONIDCMP);          // Inform us about GadTools button events

  // Setting the first gadget of the gadet list for the window
  setFirstGadget(m_pGadgetList);

  m_bInitialized = true;
}

bool FilesWindow::HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress)
{
  if(!IsOpen())
  {
    return false;
  }

  switch (p_Class)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) p_IAddress;
      if(pGadget->GadgetID == GID_LeftFileButton)
      {
        // Button "..." to select left file was clicked

        // Read latest string gadgets contents before continue
        readStringGadgetsText();

        // Open an ASL file request to let the user select the file
        if(selectFile(m_LeftFilePath, "Select left (original) file"))
        {
          setStringGadgetText(m_pLeftFileStringGadget, m_LeftFilePath);
          setDiffButtonState();
        }
      }
      else if(pGadget->GadgetID == GID_RightFileButton)
      {
        // Button "..." to select right file was clicked

        // Read latest string gadgets contents before continue
        readStringGadgetsText();

        // Open an ASL file request to let the user select the file
        if(selectFile(m_RightFilePath, "Select right (changed) file"))
        {
          setStringGadgetText(m_pRightFileStringGadget, m_RightFilePath);
          setDiffButtonState();
        }
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
        if(m_LeftFilePath.Length() == 0 ||
           m_RightFilePath.Length() == 0)
        {
          // Note: true marks that the *idcmp* was handled properly,
          //       regardless of the diff not being performed
          return true;
        }

        // Perform the diff
        m_CmdDiff.Execute();
/*
        if(m_DiffFacade.Diff() == true)
        {
          // Diff was successful. Left and right diff windows should
          // be open now, so this window can be closed
          enableAll();
          Close();
        }
        else
        {
          // TODO FileRequest to inform the user about diff error
          enableAll();
        }
*/
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

bool FilesWindow::selectFile(SimpleString& p_FilePath,
  const SimpleString& p_RequestTitle)
{
  if(m_bFileRequestOpen)
  {
    // Shouldn't be possible, but anyhow
    return false;
  }

  m_bFileRequestOpen = true;
  disableAll();

  AslFileRequest request(IntuiWindow());
  SimpleString selectedFile = request.SelectFile(p_RequestTitle, p_FilePath);

  if(selectedFile.Length() == 0)
  {
    // Cancelled or nothing selected
    m_bFileRequestOpen = false;
    enableAll();
    return false;
  }

  // Note: This will copy selectedFile to the target of p_FilePath
  //       (the variable p_FilePath points to).
  //       @see  SimpleString& operator=(const SimpleString& p_Other);
  //
  //       The reference itself is not rebound to p_FilePath as by
  //       design refereces can't be rebound after initialization.
  //
  //       Because of the copying p_FilePath will 'point to' the valid
  //       copied value of selectedFile after leaving the method scope.
  p_FilePath = selectedFile;

  enableAll();
  m_bFileRequestOpen = false;
  return true;
}

void FilesWindow::enableAll()
{
  GT_SetGadgetAttrs(m_pLeftFileStringGadget, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pRightFileStringGadget, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pSelectLeftFileButton, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pSelectRightFileButton, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  GT_SetGadgetAttrs(m_pCancelButton, IntuiWindow(), NULL,
    GA_Disabled, FALSE,
    TAG_END);

  setDiffButtonState();

  // TODO enable menu item "Quit"
}

void FilesWindow::disableAll()
{
  GT_SetGadgetAttrs(m_pLeftFileStringGadget, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pRightFileStringGadget, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pSelectLeftFileButton, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pSelectRightFileButton, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pCancelButton, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  GT_SetGadgetAttrs(m_pDiffButton, IntuiWindow(), NULL,
    GA_Disabled, TRUE,
    TAG_END);

  // TODO disable menu item "Quit"
}


void FilesWindow::setDiffButtonState()
{
  if(!IsOpen() || m_pDiffButton == NULL)
  {
    return;
  }

  if(m_LeftFilePath.Length() > 0 &&
     m_RightFilePath.Length() > 0)
  {
    // Enable "Diff" button
    GT_SetGadgetAttrs(m_pDiffButton, m_pWindow, NULL,
      GA_Disabled, FALSE,
      TAG_END);
  }
  else
  {
    // Disable "Diff" button
    GT_SetGadgetAttrs(m_pDiffButton, m_pWindow, NULL,
      GA_Disabled, TRUE,
      TAG_END);

  }
}

void FilesWindow::setStringGadgetText(struct Gadget* p_pGadget,
  const SimpleString& p_Text)
{
  if(!IsOpen() || p_pGadget == NULL)
  {
    return;
  }

  GT_SetGadgetAttrs(p_pGadget, m_pWindow, NULL,
    GTST_String, p_Text.C_str(),
    TAG_END);
}

void FilesWindow::readStringGadgetsText()
{
  // Set the changed string gadget text as left file path
  UBYTE* pBuf = ((struct StringInfo*)
    m_pLeftFileStringGadget->SpecialInfo)->Buffer;

  m_LeftFilePath = (const char*)pBuf;

  // Set the changed string gadget text as right file path
  pBuf = ((struct StringInfo*)
    m_pRightFileStringGadget->SpecialInfo)->Buffer;

  m_RightFilePath = (const char*)pBuf;

  // Enable the 'Diff' button when both string gadgets contain text.
  // If not not: disable it.
  setDiffButtonState();

}