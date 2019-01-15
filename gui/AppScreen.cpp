#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <graphics/text.h>
#include "AppScreen.h"


AppScreen::AppScreen()
  :
    m_pScreen(NULL),
    m_pDrawInfo(NULL),
    m_pTextFont(NULL),
    m_FontName(""),
    m_pVisualInfo(NULL),
    m_Title("AppScreen")
{
}

AppScreen::~AppScreen()
{
  Close();
}

bool AppScreen::Open(ScreenModeEasy p_ScreenModeEasy,
  SimpleString p_PubScreenName)
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
  m_PubScreenName = p_PubScreenName;

  if(m_ScreenModeEasy != SME_UseNamedPubScreen)
  {
    m_PubScreenName = "Workbench";
  }

  if((m_ScreenModeEasy == SME_UseNamedPubScreen) &&
     (m_PubScreenName.Length() == 0))
  {
    m_ScreenModeEasy = SME_CloneWorkbenchMin8Col;
  }

  //
  // Locking the given public screen (or workbench) and get some
  // needed data from it
  //
  struct Screen* pPubScr = m_pScreen = LockPubScreen(m_PubScreenName.C_str());
  if(pPubScr == NULL)
  {
    return false;
  }

  struct DrawInfo* pPubScrDrawInfo = m_pDrawInfo = GetScreenDrawInfo(pPubScr);
  if(pPubScrDrawInfo == NULL)
  {
    UnlockPubScreen(NULL, pPubScr);
    return false;
  }

  ULONG publicScreenModeId = GetVPModeID(&pPubScr->ViewPort);
  if(publicScreenModeId == INVALID_ID)
  {
    FreeScreenDrawInfo(pPubScr, pPubScrDrawInfo);
    UnlockPubScreen(NULL, pPubScr);
    return false;
  }

  // Get font font name and other properties from the public screens
  // DrawInfo and store it here
  m_FontName = pPubScrDrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
  m_TextAttr.ta_Name = const_cast<STRPTR>(m_FontName.C_str());
  m_TextAttr.ta_YSize = pPubScrDrawInfo->dri_Font->tf_YSize;
  m_TextAttr.ta_Style = pPubScrDrawInfo->dri_Font->tf_Style;
  m_TextAttr.ta_Flags = pPubScrDrawInfo->dri_Font->tf_Flags;

  m_pTextFont = OpenFont(&m_TextAttr);
  if(m_pTextFont == NULL)
  {
    FreeScreenDrawInfo(pPubScr, pPubScrDrawInfo);
    UnlockPubScreen(NULL, pPubScr);
    return false;
  }

  if((m_ScreenModeEasy == AppScreen::SME_UseWorkbench) ||
     (m_ScreenModeEasy == AppScreen::SME_UseNamedPubScreen))
  {
    //
    // Using the Workbench or an other  public screen
    //

    m_pScreen = pPubScr;
    m_pDrawInfo = pPubScrDrawInfo;
  }
  else
  {
    //
    // Creating a copy of the Workbench screen
    //

    // Ensure that screen has at least 8 colors
    int screenDepth = pPubScrDrawInfo->dri_Depth;

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
      SA_Width, pPubScr->Width,
      SA_Height, pPubScr->Height,
      SA_Depth, screenDepth,
      SA_Overscan, OSCAN_TEXT,
      SA_AutoScroll, TRUE,
      SA_Pens, (ULONG)pPubScrDrawInfo->dri_Pens,
      SA_Font, (ULONG) &m_TextAttr,
      SA_SharePens, TRUE,
      SA_DisplayID, publicScreenModeId,
      SA_Title, m_Title.C_str(),
      TAG_DONE);

    // We don't need them anymore
    FreeScreenDrawInfo(pPubScr, pPubScrDrawInfo);
    UnlockPubScreen(NULL, pPubScr);

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

  // Freeing the allocated pens before closing the screen as the intui
  // screen is needed there
  m_Pens.Dispose();

  if(m_pTextFont != NULL)
  {
    CloseFont(m_pTextFont);
    m_pTextFont = NULL;
  }

  if(m_pDrawInfo != NULL && m_pScreen != NULL)
  {
    FreeScreenDrawInfo(m_pScreen, m_pDrawInfo);
    m_pDrawInfo = NULL;
  }

  if(m_pScreen != NULL)
  {
    if((m_ScreenModeEasy == AppScreen::SME_UseWorkbench) ||
       (m_ScreenModeEasy == AppScreen::SME_UseNamedPubScreen))
    {
      // We had used a public screen or the Workbench
      UnlockPubScreen(NULL, m_pScreen);
    }
    else
    {
      // We had created a copy of the Workbench screen
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