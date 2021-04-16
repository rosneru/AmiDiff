#ifdef __clang__
  #include <clib/dos_protos.h>
  #include <clib/exec_protos.h>
  #include <clib/gadtools_protos.h>
  #include <clib/graphics_protos.h>
  #include <clib/intuition_protos.h>
#else
  #include <proto/dos.h>
  #include <proto/exec.h>
  #include <proto/gadtools.h>
  #include <proto/graphics.h>
  #include <proto/intuition.h>
#endif

#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include <workbench/startup.h>

#include <string.h>
#include <stdio.h>
#include <vector>

#include "SearchWindow.h"


  /**
   * Array of string constants for 'Location' cycle gadget
   */
  const char* g_GadCycLocationLabels [4] = {"Both files", "Left file", "Right file", NULL};



SearchWindow::SearchWindow(std::vector<WindowBase*>& windowArray,
                         ScreenBase& screen,
                         struct MsgPort* pIdcmpMsgPort,
                         CommandBase& cmdSearch,
                         CommandBase& cmdCloseSearchWindow)
  : WindowBase(screen, pIdcmpMsgPort, NULL),
    m_CmdSearch(cmdSearch),
    m_CmdCloseSearchWindow(cmdCloseSearchWindow),
    m_MaxPathLength(1024),
    m_pGadtoolsContext(NULL),
    m_pGadStrSearchText(NULL),
    m_pGadCycLocation(NULL),
    m_pGadCbxIgnoreCase(NULL),
    m_pGadBtnFind(NULL)
{
  const char* pErrMsg = "SearchWindow: Failed to create gadgets.";
  //
  // Calculate some basic values
  //
  struct Screen* pIntuiScreen = m_Screen.IntuiScreen();
  UWORD fontHeight = m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;

  // Default button dimensions
  WORD btnsHeight = fontHeight + 6;

  WORD hSpace = 10;
  WORD vSpace = 6;


  // as default the window's width should be half of the screen's width
  m_Width = (WORD)pIntuiScreen->Width / 2;
  if(m_Width < 400)
  {
    //  but at least 400
    m_Width = 400;
  }

  WORD barHeight = pIntuiScreen->WBorTop + fontHeight + 2;

  WORD top = barHeight + vSpace;
  WORD left = pIntuiScreen->WBorLeft + hSpace;
  WORD right = m_Width - pIntuiScreen->WBorRight - hSpace - hSpace;

  // Set the labelWidth to the longest label text
  const char* labelTexts[]  = {"Search for", "Location", "Ignore case"};
  size_t arraySize = sizeof(labelTexts) / (sizeof labelTexts[0]);
  WORD labelsWidth = maxArrayTextLength(labelTexts, arraySize);

  WORD contentWidth = right - left - labelsWidth;

  arraySize = sizeof(g_GadCycLocationLabels) / sizeof(g_GadCycLocationLabels[0]);
  WORD cycWidth = maxArrayTextLength(g_GadCycLocationLabels, arraySize);
  cycWidth += 3 * hSpace;

  //
  // Set up the gadgets
  //

  // Create a place for GadTools context data
  struct Gadget* pFakeGad;
  pFakeGad = (struct Gadget*) CreateContext(&m_pGadtoolsContext);
  if(pFakeGad == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Set the first gadget of the gadet list for the window
  setFirstGadget(m_pGadtoolsContext);

  // Declare the basic gadget structure
  struct NewGadget newGadget;

  // Create the string gadget for the search text
  newGadget.ng_TextAttr   = m_Screen.IntuiTextAttr();
  newGadget.ng_VisualInfo = m_Screen.GadtoolsVisualInfo();
  newGadget.ng_Flags      = NG_HIGHLABEL;
  newGadget.ng_LeftEdge   = labelsWidth + left + hSpace;
  newGadget.ng_TopEdge    = top;
  newGadget.ng_Width      = contentWidth;
  newGadget.ng_Height     = fontHeight + 5;
  newGadget.ng_GadgetText = (UBYTE*) "_Search for";
  newGadget.ng_GadgetID   = GID_StrSearchText;

  m_pGadStrSearchText = CreateGadget(STRING_KIND,
                                   pFakeGad,
                                   &newGadget,
                                   GTST_MaxChars, m_MaxPathLength,
                                   GT_Underscore, '_',
                                   TAG_DONE);
  if(m_pGadStrSearchText == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  newGadget.ng_TopEdge    += btnsHeight + vSpace;
  newGadget.ng_Width      = cycWidth;
  newGadget.ng_GadgetText = (UBYTE*) "_Location";
  newGadget.ng_GadgetID   = GID_CycLocation;

  m_pGadCycLocation = CreateGadget(CYCLE_KIND,
                                   m_pGadStrSearchText,
                                   &newGadget,
                                   GT_Underscore, '_',
                                   GTCY_Labels, (ULONG)g_GadCycLocationLabels,
                                   TAG_DONE);

  if(m_pGadCycLocation == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Create the 'Ignore case' checkbox
  newGadget.ng_LeftEdge   += (cycWidth + hSpace + hSpace + labelsWidth);
  newGadget.ng_TopEdge    ++; // Manually center text vertically as 
                              // GadTools doesn't do it

  newGadget.ng_Width      = right - newGadget.ng_LeftEdge;
  newGadget.ng_GadgetText = (UBYTE*) "Ignore _case";
  newGadget.ng_GadgetID   = GID_CbxIgnoreCase;

  m_pGadCycLocation = CreateGadget(CHECKBOX_KIND,
                                   m_pGadCycLocation,
                                   &newGadget,
                                   GT_Underscore, '_',
                                   GTCB_Checked, TRUE,
                                   TAG_DONE);

  if(m_pGadCycLocation == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Create the 'Find' button
  newGadget.ng_TopEdge    += (btnsHeight + vSpace + vSpace - 1);
  newGadget.ng_LeftEdge   = left + hSpace;
  newGadget.ng_Width      = cycWidth;
  newGadget.ng_GadgetText = (UBYTE*) "Find";
  newGadget.ng_GadgetID   = GID_BtnFind;

  m_pGadBtnFind = CreateGadget(BUTTON_KIND,
                               m_pGadCycLocation,
                               &newGadget,
                               TAG_DONE);
  if(m_pGadBtnFind == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_Height = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  SetTitle("Find");

  // Setting the window flags
  addFlags(WFLG_CLOSEGADGET |     // Add a close gadget
           WFLG_DRAGBAR |         // Add a drag gadget
           WFLG_DEPTHGADGET);     // Add a depth gadget

  // Setting the IDCMP messages we want to receive for this window
  addIDCMP(IDCMP_MENUPICK |       // Get msg when menu selected
           IDCMP_VANILLAKEY |     // Get msg when RAW key pressed
           IDCMP_CLOSEWINDOW |    // Get msg when close gadget clicked
           IDCMP_REFRESHWINDOW |  // Get msg when must refresh; needed by GadTools
           IDCMP_GADGETUP);       // Get msg when gadgets state changed
}


SearchWindow::~SearchWindow()
{
  cleanup();
}


void SearchWindow::Refresh()
{
// TODO
//  BeginRefresh(m_pWindow);
//  EndRefresh(m_pWindow, TRUE);
}

bool SearchWindow::Open(InitialPosition initialPos)
{
  if(!WindowBase::Open(initialPos))
  {
    return false;
  }

  // Enable or disable the 'Diff' and 'Swap' buttons depending on some
  // conditions
  checkEnableButtons();

  return true;
}


void SearchWindow::HandleIdcmp(ULONG msgClass,
                              UWORD msgCode,
                              APTR pItemAddress)
{
  if(!IsOpen())
  {
    return;
  }

  switch (msgClass)
  {
    case IDCMP_GADGETUP:
    {
      struct Gadget* pGadget = (struct Gadget*) pItemAddress;
      handleGadgetEvent(pGadget);
      break;
    }

    case IDCMP_REFRESHWINDOW:
    {
      // This handling is REQUIRED with GadTools
      GT_BeginRefresh(m_pWindow);
      GT_EndRefresh(m_pWindow, TRUE);
      break;
    }

    case IDCMP_CLOSEWINDOW:
    {
      m_CmdCloseSearchWindow.Execute(NULL);
      break;
    }

    case IDCMP_VANILLAKEY:
    {
      handleVanillaKey(msgCode);
      break;
    }
  }
}


void SearchWindow::handleGadgetEvent(struct Gadget* pGadget)
{
  if(pGadget == NULL)
  {
    return;
  }

  switch(pGadget->GadgetID)
  {
    case GID_StrSearchText:
      checkEnableButtons();
      break;

    case GID_CycLocation:  // Select left file
      break;

    case GID_CbxIgnoreCase: // Select right file
      
      break;

    case GID_BtnFind:      // Compare the files and display the diff
      find();
      break;
  }
}


void SearchWindow::handleVanillaKey(UWORD code)
{
  switch(code)
  {
    case 'l': // Select left file
    case 'L':
      selectLeftFile();
      break;

    case 0xD:// <RETURN> Search
    {
      // Allow only if Find button is enabled
      long disabled;
      long numProcessed;
      numProcessed  = GT_GetGadgetAttrs(m_pGadBtnFind, m_pWindow, NULL,
                                        GA_Disabled, (ULONG)&disabled,
                                        TAG_DONE);

      if((numProcessed != 1) || (disabled == 1))
      {
        return;
      }

      // Button is enabled, perform its action
      find();
      break;
    }

    case 0x1B: // <ESC> Cancel
      m_CmdCloseSearchWindow.Execute(NULL);
      break;

  }
}


void SearchWindow::selectLeftFile()
{
  // Read latest string gadgets contents before continue
  STRPTR pLeftStrGadgetText = getStringGadgetText(m_pGadStrSearchText);

  // if((pLeftStrGadgetText == NULL) || (pRightStrGadgetText == NULL))
  // {
  //   checkEnableButtons();
  //   return;
  // }

  // setStringGadgetText(m_pGadStrSearchText,
  //                     m_CmdSelectLeftFile.SelectedFile().c_str());

  checkEnableButtons();
}



void SearchWindow::find()
{
  if(!IsOpen() || (m_pGadBtnFind == NULL))
  {
    return;
  }

  STRPTR pLeftStrGadgetText = getStringGadgetText(m_pGadStrSearchText);

  if(pLeftStrGadgetText == NULL)
  {
    return;
  }

  // Perform the diff
  m_CmdSearch.Execute(NULL);
}



void SearchWindow::cleanup()
{
  if(m_pGadtoolsContext != NULL)
  {
    FreeGadgets(m_pGadtoolsContext);
  }

  m_pGadtoolsContext = NULL;
  m_pGadStrSearchText = NULL;
  m_pGadCycLocation = NULL;
  m_pGadCbxIgnoreCase = NULL;
  m_pGadBtnFind = NULL;
}

void SearchWindow::checkEnableButtons()
{
  if(!IsOpen() || (m_pGadBtnFind == NULL))
  {
    return;
  }

  STRPTR pLeftStrGadgetText = getStringGadgetText(m_pGadStrSearchText);

  if(pLeftStrGadgetText == NULL)
  {
    return;
  }

  if(strlen(pLeftStrGadgetText) > 0)
  {
    // Enable "Find" button
    GT_SetGadgetAttrs(m_pGadBtnFind, m_pWindow, NULL,
                      GA_Disabled, FALSE,
                      TAG_DONE);
  }
  else
  {
    // Disable "Find" button
    GT_SetGadgetAttrs(m_pGadBtnFind, m_pWindow, NULL,
                      GA_Disabled, TRUE,
                      TAG_DONE);
  }
}

void SearchWindow::setStringGadgetText(struct Gadget* pGadget,
                                      const char* pText)
{
  if(!IsOpen() || (pGadget == NULL) || (pText == NULL))
  {
    return;
  }

  GT_SetGadgetAttrs(pGadget, m_pWindow, NULL,
                    GTST_String, (ULONG)pText,
                    TAG_DONE);
}

STRPTR SearchWindow::getStringGadgetText(struct Gadget* pGadget)
{
  if(!IsOpen() || pGadget == NULL)
  {
    return NULL;
  }

  long pTextPointerStorage;
  long numProcessed;

  numProcessed  = GT_GetGadgetAttrs(pGadget, m_pWindow, NULL,
                                    GTST_String, (ULONG)&pTextPointerStorage,
                                    TAG_DONE);
  if(numProcessed != 1)
  {
    return NULL;
  }

  return (STRPTR)pTextPointerStorage;
}
