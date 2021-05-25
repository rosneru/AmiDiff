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
  const char* g_GadCycStartSearchLabels [3] = {"Current page", "Document top", NULL};



SearchWindow::SearchWindow(std::vector<WindowBase*>& windowArray,
                         ScreenBase& screen,
                         struct MsgPort* pIdcmpMsgPort,
                         CmdSearch& cmdSearch,
                         CommandBase& cmdCloseSearchWindow)
  : WindowBase(screen, pIdcmpMsgPort, NULL),
    m_CmdSearch(cmdSearch),
    m_CmdCloseSearchWindow(cmdCloseSearchWindow),
    m_NumLocationLabels(sizeof(g_GadCycLocationLabels) /
                        sizeof(g_GadCycLocationLabels[0])
                        - 1), // Skip trailing NULL item
    m_NumStartSearchFromLabels(sizeof(g_GadCycStartSearchLabels) /
                        sizeof(g_GadCycStartSearchLabels[0])
                        - 1), // Skip trailing NULL item
    m_pGadtoolsContext(NULL),
    m_pGadStrSearchText(NULL),
    m_pGadCycLocation(NULL),
    m_pGadCycStartSearchFrom(NULL),
    m_pGadCbxIgnoreCase(NULL),
    m_pGadBtnFindNext(NULL),
    m_pGadBtnFindPrev(NULL)
{     
  const char* pErrMsg = "SearchWindow: Failed to create gadgets.";

  //
  // Calculate some basic values
  //
  struct Screen* pIntuiScreen = m_Screen.IntuiScreen();
  UWORD fontHeight = m_Screen.IntuiDrawInfo()->dri_Font->tf_YSize;

  WORD hSpace = 10;
  WORD vSpace = 6;

  //
  // Default button dimensions
  //

  // Extra space by which a button must be wider than its text to look good
  const int btnExtraHSpace = 8;

  // Set the same width for all of the bottom buttons row according to
  // the longest button text
  const char* btnTexts[]  = {"Find next", "Find prev"};
  size_t numBottomButtons = sizeof(btnTexts) / (sizeof btnTexts[0]);
  WORD btnsWidth = maxArrayTextLength(btnTexts, numBottomButtons);
  btnsWidth += btnExtraHSpace;

  // Buttons height
  WORD btnsHeight = fontHeight + 6;

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
  const char* labelTexts[]  = 
  { 
    "Search for", "Location", "Ignore case", "Start from"
  };

  size_t arraySize = sizeof(labelTexts) / (sizeof labelTexts[0]);
  WORD labelsWidth = maxArrayTextLength(labelTexts, arraySize);

  WORD contentWidth = right - left - labelsWidth;

  // Both cycle gadgets should be the same width. Calculate this width 
  // by measuring the longest text width

  arraySize = sizeof(g_GadCycLocationLabels) / sizeof(g_GadCycLocationLabels[0]);
  WORD cyc1Width = maxArrayTextLength(g_GadCycLocationLabels, arraySize);

  arraySize = sizeof(g_GadCycStartSearchLabels) / sizeof(g_GadCycStartSearchLabels[0]);
  WORD cyc2Width = maxArrayTextLength(g_GadCycStartSearchLabels, arraySize);

  WORD cycWidth = cyc1Width;
  if(cyc2Width > cycWidth)
  {
    cycWidth = cyc2Width;
  }

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

  // Set the first gadget of the gadget list for the window
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

  m_pGadCbxIgnoreCase = CreateGadget(CHECKBOX_KIND,
                                     m_pGadCycLocation,
                                     &newGadget,
                                     GT_Underscore, '_',
                                     GTCB_Checked, FALSE,
                                     TAG_DONE);

  if(m_pGadCbxIgnoreCase == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Create the 'Start search from' cycle gadget
  newGadget.ng_LeftEdge   = labelsWidth + left + hSpace;
  newGadget.ng_TopEdge    += btnsHeight + vSpace - 1;
  newGadget.ng_Width      = cycWidth;
  newGadget.ng_GadgetText = (UBYTE*) "Start _from";
  newGadget.ng_GadgetID   = GID_CycStartSearchFrom;
  m_pGadCycStartSearchFrom = CreateGadget(CYCLE_KIND,
                                          m_pGadCbxIgnoreCase,
                                          &newGadget,
                                          GT_Underscore, '_',
                                          GTCY_Labels, (ULONG)g_GadCycStartSearchLabels,
                                          TAG_DONE);

  // Create the 'Find next' button
  newGadget.ng_TopEdge    += btnsHeight + vSpace + vSpace;
  newGadget.ng_LeftEdge   = left + hSpace;
  newGadget.ng_Width      = btnsWidth;
  newGadget.ng_GadgetText = (UBYTE*) "Find _next";
  newGadget.ng_GadgetID   = GID_BtnFindNext;

  m_pGadBtnFindNext = CreateGadget(BUTTON_KIND,
                                   m_pGadCycStartSearchFrom,
                                   &newGadget,
                                   GT_Underscore, '_',
                                   TAG_DONE);
  if(m_pGadBtnFindNext == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Create the 'Find prev' button
  newGadget.ng_LeftEdge  += btnsWidth;
  if(newGadget.ng_LeftEdge < (labelsWidth + left + hSpace))
  {
    // Hope this h-distance is enough. But it is visually a good idea
    // to start the button on a vertical line with the gadgets above.
    newGadget.ng_LeftEdge = labelsWidth + left + hSpace;
  }
  else
  {
    // Add a h-distance to previous button
    newGadget.ng_LeftEdge += 10;
  }

  newGadget.ng_GadgetText = (UBYTE*) "Find _prev";
  newGadget.ng_GadgetID   = GID_BtnFindPrev;

  m_pGadBtnFindPrev = CreateGadget(BUTTON_KIND,
                                   m_pGadBtnFindNext,
                                   &newGadget,
                                   GT_Underscore, '_',
                                   TAG_DONE);

  if(m_pGadBtnFindPrev == NULL)
  {
    cleanup();
    throw pErrMsg;
  }

  // Adjust the window height depending on the y-Pos and height of the
  // last gadget
  m_Height = newGadget.ng_TopEdge + newGadget.ng_Height + vSpace;

  // Setting window title
  setTitle("Find");

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

bool SearchWindow::open(InitialPosition initialPos)
{
  /**
   * If this is the first opening of search window (when no search text
   * is present in cmdSearch) then apply the options from search command
   * before opening.
   *
   * NOTE: This can and should be done before opening the window to
   * avoid possible visible changing of the cycle boxes when the window
   * already is open
   *
   * NOTE: No check of gadget pointers needed. If there would be
   * something wrong the constructor had already thrown an exception and
   * this wouldn't be executed.
   */

  if(strlen(m_CmdSearch.getSearchText()) < 1)
  {
    setFindButtonsEnabled(false);

    if(m_CmdSearch.isCaseIgnored())
    {
      GT_SetGadgetAttrs(m_pGadCbxIgnoreCase, m_pWindow, NULL,
                        GTCB_Checked, TRUE,
                        TAG_DONE);
    }
    else
    {
      GT_SetGadgetAttrs(m_pGadCbxIgnoreCase, m_pWindow, NULL,
                        GTCB_Checked, FALSE,
                        TAG_DONE);
    }

    switch(m_CmdSearch.getLocation())
    {
      case SL_BothFiles:
      {
        GT_SetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                          GTCY_Active, 0, // TODO Change manual value; use map?
                          TAG_DONE);
        break;
      }

      case SL_LeftFile:
      {
        GT_SetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                          GTCY_Active, 1, // TODO Change manual value; use map?
                          TAG_DONE);
        break;
      }

      case SL_RightFile:
      {
        GT_SetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                          GTCY_Active, 2, // TODO Change manual value; use map?
                          TAG_DONE);
        break;
      }
    }

    switch(m_CmdSearch.getStartFrom())
    {
      case SF_CurrentPage:
      {
        GT_SetGadgetAttrs(m_pGadCycStartSearchFrom, m_pWindow, NULL,
                          GTCY_Active, 0, // TODO Change manual value; use map?
                          TAG_DONE);
        break;
      }

      case SF_DocumentTop:
      {
        GT_SetGadgetAttrs(m_pGadCycStartSearchFrom, m_pWindow, NULL,
                          GTCY_Active, 1, // TODO Change manual value; use map?
                          TAG_DONE);
        break;
      }
    }
  }
  else
  {
    setFindButtonsEnabled(true);
  }

  //
  // Open the window
  //
  if(!WindowBase::open(initialPos))
  {
    return false;
  }

  // Also on when opened for the first time, the search text gadget
  // should be activated
  if(strlen(m_CmdSearch.getSearchText()) < 1)
  {
    ActivateGadget(m_pGadStrSearchText, m_pWindow, NULL);
  }

  return true;
}


void SearchWindow::handleIDCMP(ULONG msgClass,
                              UWORD msgCode,
                              APTR pItemAddress)
{
  if(!isOpen())
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
    {
      STRPTR pTextToFind = getStringGadgetText(m_pGadStrSearchText);
      if(pTextToFind == NULL)
      {
        return;
      }

      // Set the user-typed text to find in search command
      m_CmdSearch.setSearchText(pTextToFind);

      // Disable / enable find buttons according to weather there is
      // search text available or not
      if(strlen(pTextToFind) < 1)
      {
        setFindButtonsEnabled(false);
      }
      else
      {
        setFindButtonsEnabled(false);
      }

      break;
    }

    case GID_CycLocation:
    {
      // Get the current index value from 'location' cycle gadget
      ULONG currentId = 0;
      if(GT_GetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                          GTCY_Active, (ULONG)&currentId,
                          TAG_DONE) != 1)
      {
        return;
      }

      m_CmdSearch.setLocation((SearchLocation)currentId);

      break;
    }

    case GID_CycStartSearchFrom:
    {
      // Get the current index value from 'start search from' cycle gadget
      ULONG currentId = 0;
      if(GT_GetGadgetAttrs(m_pGadCycStartSearchFrom, m_pWindow, NULL,
                          GTCY_Active, (ULONG)&currentId,
                          TAG_DONE) != 1)
      {
        return;
      }

      m_CmdSearch.setStartFrom((StartSearchFrom)currentId);

      break;
    }

    case GID_CbxIgnoreCase:
    {
      // Get the current state of 'is case ignored' checkbox gadget
      ULONG isChecked = FALSE;
      if(GT_GetGadgetAttrs(m_pGadCbxIgnoreCase, m_pWindow, NULL,
                          GTCB_Checked, (ULONG)&isChecked,
                          TAG_DONE) != 1)
      {
        return;
      }

      m_CmdSearch.setCaseIgnored(isChecked == TRUE);

      break;
    }

    case GID_BtnFindNext:
    {
      // Set the search direction and search
      m_CmdSearch.setDirection(SD_Downward);
      m_CmdSearch.Execute(NULL);
      break;
    }

    case GID_BtnFindPrev:
    {
      // Set the search direction and search
      m_CmdSearch.setDirection(SD_Upward);
      m_CmdSearch.Execute(NULL);
      break;
    } 
  }
}


void SearchWindow::handleVanillaKey(UWORD code)
{
  switch(code)
  {
    // case 0xD: // <RETURN> Find
    // {
    //   // Button is enabled, perform its action
    //   find();
    //   break;
    // }

    case 'f':
    case 'F':
    {
      toggleStartSearchFromGadget();
      break;
    }

    case 'l':
    case 'L':
    {
      toggleLocationGadget();
      break;
    }

    case 's':
    case 'S':
    {
      ActivateGadget(m_pGadStrSearchText, m_pWindow, NULL);
      break;
    }


    case 0x1B: // <ESC> Cancel
      m_CmdCloseSearchWindow.Execute(NULL);
      break;

  }
}


void SearchWindow::toggleLocationGadget()
{
  // Get the current index value from 'location' cycle gadget
  ULONG currentId = 0;
  if(GT_GetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                      GTCY_Active, (ULONG)&currentId,
                      TAG_DONE) != 1)
  {
    return;
  }

  // Increase the id; start from 0 when over max
  currentId++;
  if(currentId >= m_NumLocationLabels)
  {
    currentId = 0;
  }

  // Set the new index value in gadget
  GT_SetGadgetAttrs(m_pGadCycLocation, m_pWindow, NULL,
                    GTCY_Active, currentId,
                    TAG_DONE);
}

void SearchWindow::toggleStartSearchFromGadget()
{
  // Get the current index value from 'location' cycle gadget
  ULONG currentId = 0;
  if(GT_GetGadgetAttrs(m_pGadCycStartSearchFrom, m_pWindow, NULL,
                       GTCY_Active, (ULONG)&currentId,
                       TAG_DONE) != 1)
  {
    return;
  }

  // Increase the id; start from 0 when over max
  currentId++;
  if(currentId >= m_NumStartSearchFromLabels)
  {
    currentId = 0;
  }

  // Set the new index value in gadget
  GT_SetGadgetAttrs(m_pGadCycStartSearchFrom, m_pWindow, NULL,
                    GTCY_Active, currentId,
                    TAG_DONE);
}


void SearchWindow::setFindButtonsEnabled(bool enabled)
{
  BOOL disabled;
  if(enabled == true)
  {
    disabled = FALSE;
  }
  else
  {
    disabled = TRUE;
  }

  GT_SetGadgetAttrs(m_pGadBtnFindNext, m_pWindow, NULL,
                    GA_Disabled, disabled,
                    TAG_DONE);

  GT_SetGadgetAttrs(m_pGadBtnFindPrev, m_pWindow, NULL,
                    GA_Disabled, disabled,
                    TAG_DONE);

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
  m_pGadBtnFindNext = NULL;
}
