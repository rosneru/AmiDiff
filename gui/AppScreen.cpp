#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <graphics/text.h>
#include "AppScreen.h"


AppScreen::AppScreen(SimpleString p_pPubScreenName)
  : m_PubScreenName(p_pPubScreenName),
    m_pTextFont(NULL),
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
  //
  // Initial validations
  //

  if(m_pScreen != NULL)
  {
    // Not opening the screen if it is already open
    return false;
  }

  m_ScreenModeEasy = p_ScreenModeEasy;
  if(m_ScreenModeEasy == SME_UseWorkbench)
  {
    m_PubScreenName = "Workbench";
  }

  if((m_ScreenModeEasy == SME_UseNamedPubScreen) &&
     (m_PubScreenName.Length == 0))
  {
    m_ScreenModeEasy = SME_CloneWorkbenchMin8Col;
  }

  //
  // Get some data from the Workbench screen to open an own screen with
  // similar parameters
  //
  struct Screen* pPublicScreen = m_pScreen = LockPubScreen(m_PubScreenName.C_str();
  if(pPublicScreen == NULL)
  {
    return false;
  }

  struct DrawInfo* pPublicScreenDrawInfo = m_pDrawInfo = GetScreenDrawInfo(pPublicScreen);
  if(pPublicScreenDrawInfo == NULL)
  {
    UnlockPubScreen(NULL, pPublicScreen);
    return false;
  }

  ULONG wbScreenModeId = GetVPModeID(&pPublicScreen->ViewPort);
  if(wbScreenModeId == INVALID_ID)
  {
    FreeScreenDrawInfo(pPublicScreen, pPublicScreenDrawInfo);
    UnlockPubScreen(NULL, pPublicScreen);
    return false;
  }

  // Get font font name and other properties fro Workbench DrawInfo and
  // store it here
  m_FontName = pPublicScreenDrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_Name = const_cast<STRPTR>(m_FontName.C_str());
  m_TextAttr.ta_YSize = pPublicScreenDrawInfo->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = pPublicScreenDrawInfo->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = pPublicScreenDrawInfo->dri_Font->tf_Flags;

  m_pTextFont = OpenFont(&m_TextAttr);
  if(m_pTextFont == NULL)
  {
    FreeScreenDrawInfo(pPublicScreen, pPublicScreenDrawInfo);
    UnlockPubScreen(NULL, pPublicScreen);
    return false;
  }

  if((m_ScreenModeEasy == AppScreen::SME_UseWorkbench) ||
     (m_ScreenModeEasy == AppScreen::SME_UseNamedPubScreen)
  {
    //
    // Using the Workbench or an other  public screen
    //

    m_pScreen = pPublicScreen;
    m_pDrawInfo = pPublicScreenDrawInfo;
  }
  else
  {
    //
    // Creating a copy of the Workbench screen
    //

    // Ensure that screen has at least 8 colors
    int screenDepth = pPublicScreenDrawInfo->dri_Depth;

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
      SA_Width, pPublicScreen->Width,
      SA_Height, pPublicScreen->Height,
      SA_Depth, screenDepth,
      SA_Overscan, OSCAN_TEXT,
      SA_AutoScroll, TRUE,
      SA_Pens, (ULONG)pPublicScreenDrawInfo->dri_Pens,
      SA_Font, (ULONG) &m_TextAttr,
      SA_DisplayID, wbScreenModeId,
      SA_Title, m_Title.C_str(),
      TAG_DONE);

    // We don't need them anymore
    FreeScreenDrawInfo(pPublicScreen, pPublicScreenDrawInfo);
    UnlockPubScreen(NULL, pPublicScreen);

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