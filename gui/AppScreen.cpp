#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/text.h>
#include "AppScreen.h"


AppScreen::AppScreen()
  : m_pTextFont(NULL),
    m_FontName(""),
    m_Title("AppScreen"),
    m_pScreen(NULL),
    m_pDrawInfo(NULL),
    m_pVisualInfo(NULL)
{
}

AppScreen::~AppScreen()
{
  Close();
}

bool AppScreen::Open(ScreenModeEasy p_ScreenModeEasy)
{
  m_ScreenModeEasy = p_ScreenModeEasy;

  //
  // Initial validations
  //

  if(m_pScreen != NULL)
  {
    // Not opening the screen if it is already open
    return false;
  }

  //
  // Get some data from the Workbench screen to open an own screen with
  // similar parameters
  //
  struct Screen* pWbScreen = m_pScreen = LockPubScreen("Workbench");
  if(pWbScreen == NULL)
  {
    return false;
  }

  struct DrawInfo* pWbDrawInfo = m_pDrawInfo = GetScreenDrawInfo(pWbScreen);
  if(pWbDrawInfo == NULL)
  {
    UnlockPubScreen(NULL, pWbScreen);
    return false;
  }

  ULONG wbScreenModeId = GetVPModeID(&pWbScreen->ViewPort);
  if(wbScreenModeId == INVALID_ID)
  {
    FreeScreenDrawInfo(pWbScreen, pWbDrawInfo);
    UnlockPubScreen(NULL, pWbScreen);
    return false;
  }

  // Get font font name and other properties fro Workbench DrawInfo and
  // store it here
  m_FontName = pWbDrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_Name = const_cast<STRPTR>(m_FontName.C_str());
  m_TextAttr.ta_YSize = pWbDrawInfo->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = pWbDrawInfo->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = pWbDrawInfo->dri_Font->tf_Flags;

  m_pTextFont = OpenFont(&m_TextAttr);
  if(m_pTextFont == NULL)
  {
    FreeScreenDrawInfo(pWbScreen, pWbDrawInfo);
    UnlockPubScreen(NULL, pWbScreen);
    return false;
  }

  if(m_ScreenModeEasy != AppScreen::SME_UseWorkbench)
  {
    //
    // Creating a copy of the Workbench screen
    //

    // Ensure that screen has at least 8 colors
    int screenDepth = pWbDrawInfo->dri_Depth;

    if(m_ScreenModeEasy == AppScreen::SME_CloneWorkbenchMin8Col)
    {
      if(screenDepth < 3)
      {
        // Ensuring 3 bitplanes
        screenDepth = 3;
      }
    }

    //
    // Opening the screen
    //
    m_pScreen = OpenScreenTags(NULL,
      SA_Width, pWbScreen->Width,
      SA_Height, pWbScreen->Height,
      SA_Depth, screenDepth,
      SA_Overscan, OSCAN_TEXT,
      SA_AutoScroll, TRUE,
      SA_Pens, (ULONG)pWbDrawInfo->dri_Pens,
      // SA_Font, (ULONG) &m_TextAttr,
      SA_SysFont, 0,  // Check if this fits
      SA_DisplayID, wbScreenModeId,
      SA_Title, m_Title.C_str(),
      TAG_DONE);

    // We don't need them anymore
    FreeScreenDrawInfo(pWbScreen, pWbDrawInfo);
    UnlockPubScreen(NULL, pWbScreen);

    if(m_pScreen == NULL)
    {
      return false;
    }

    m_pDrawInfo = GetScreenDrawInfo(m_pScreen);
    if(m_pDrawInfo == NULL)
    {
      Close();
      return false;
    }
  }
  else
  {
    //
    // Using the Workbench public screen
    //

    m_pScreen = pWbScreen;
    m_pDrawInfo = pWbDrawInfo;
  }

  // Trying to initialize our four needed color pens
  if(m_Pens.Init(this) == false)
  {
    return false;
  }

  // Get visual info from screen
  m_pVisualInfo = (APTR*) GetVisualInfo(m_pScreen, TAG_END);
  if(m_pVisualInfo == NULL)
  {
    Close();
    return false;
  }

  return true;
}

void AppScreen::Close()
{
  if(m_pVisualInfo != NULL)
  {
    FreeVisualInfo(m_pVisualInfo);
    m_pVisualInfo = NULL;
  }

  if(m_pDrawInfo != NULL && m_pScreen != NULL)
  {
    FreeScreenDrawInfo(m_pScreen, m_pDrawInfo);
    m_pDrawInfo = NULL;
  }

  if(m_pTextFont != NULL)
  {
    CloseFont(m_pTextFont);
    m_pTextFont = NULL;
  }

  // Freeing the allocated pens before closing the screen as the intui
  // screen is needed there
  m_Pens.Dispose();

  if(m_pScreen != NULL)
  {
    if(m_ScreenModeEasy == AppScreen::SME_UseWorkbench)
    {
      // We had used the Workbench public screen
      UnlockPubScreen(NULL, m_pScreen);
    }
    else
    {
      // We had created a copy of the workbench screen
      CloseScreen(m_pScreen);
    }

    m_pScreen = NULL;
  }
}

bool AppScreen::IsOpen() const
{
  return (m_pScreen != NULL);
}

const char* AppScreen::Title() const
{
  return m_Title.C_str();
}

void AppScreen::SetTitle(SimpleString p_NewTitle)
{
  if(IsOpen())
  {
    // Screen is already open, so we don't change its title dynamically
    return;
  }

  m_Title = p_NewTitle;
}

WORD AppScreen::FontHeight() const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Font->tf_YSize;
}

WORD AppScreen::BarHeight() const
{
  if(m_pScreen == NULL)
  {
    return 0;
  }

	return m_pScreen->WBorTop + FontHeight() + 2;
}

struct Screen* AppScreen::IntuiScreen()
{
  return m_pScreen;
}

struct DrawInfo* AppScreen::IntuiDrawInfo()
{
  return m_pDrawInfo;
}

struct TextAttr* AppScreen::GfxTextAttr()
{
  return &m_TextAttr;
}

APTR* AppScreen::GadtoolsVisualInfo()
{
  return m_pVisualInfo;
}

const AmigaDiffPens& AppScreen::Pens() const
{
  return m_Pens;
}

AppScreen::ScreenModeEasy AppScreen::ScreenMode() const
{
  return m_ScreenModeEasy;
}