#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/text.h>
#include <intuition/intuitionbase.h>
#include "AScreen.h"

extern struct IntuitionBase* IntuitionBase;

AScreen::AScreen(ADiffViewSettings& settings)
  : m_Pens(m_pScreen, m_pDrawInfo, settings),
    m_Settings(settings),
    m_pScreen(NULL),
    m_pDrawInfo(NULL),
    m_pVisualInfo(NULL),
    m_Title("AScreen"),
    m_PubScreenName("Workbench")
{
}

AScreen::~AScreen()
{
  Close();
}

bool AScreen::Open(ScreenModeEasy screenModeEasy,
                     SimpleString pubScreenName)
{
  if(m_pScreen != NULL)
  {
    // Not opening the screen if it is already open
    return false;
  }

  m_ScreenModeEasy = screenModeEasy;

  // Only overwrite default public screen "Workbench" when given name
  // is not empty
  if(pubScreenName.Length() > 0)
  {
    m_PubScreenName = pubScreenName;
  }

  // Fallback if no public screen name provided
  if((m_ScreenModeEasy == SME_UseNamedPubScreen)
   &&(m_PubScreenName.Length() == 0))
  {
    m_ScreenModeEasy = SME_CloneWorkbench8Col;
  }

  if((m_ScreenModeEasy == AScreen::SME_UseWorkbench) ||
     (m_ScreenModeEasy == AScreen::SME_UseNamedPubScreen))
  {
    //
    // Using the Workbench or an other public screen
    //
    m_pScreen = LockPubScreen(m_PubScreenName.C_str());
  }
  else
  {
    //
    // Opening a nearly-copy of the Workbench screen
    //
    // It is only *nearly* a copy because it'll have 8 colors instead
    // of how many the Workbench screen might have.
    //
    m_pScreen = OpenScreenTags(NULL,
                               SA_LikeWorkbench, TRUE,
                               SA_Type, CUSTOMSCREEN,
                               SA_Depth, 3,
                               SA_SharePens,TRUE,
                               SA_Title, m_Title.C_str(),
                               SA_Colors32, m_Settings.GetColorArray(),
                               TAG_DONE);
  }

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

  // Trying to initialize our four needed color pens
  if(m_Pens.Create() == false)
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

void AScreen::Close()
{
  if(m_pVisualInfo != NULL)
  {
    FreeVisualInfo(m_pVisualInfo);
    m_pVisualInfo = NULL;
  }

  // Freeing the allocated pens before closing the screen as the intui
  // screen is needed there
  m_Pens.Dispose();

  if(m_pDrawInfo != NULL && m_pScreen != NULL)
  {
    FreeScreenDrawInfo(m_pScreen, m_pDrawInfo);
    m_pDrawInfo = NULL;
  }

  if(m_pScreen != NULL)
  {
    if((m_ScreenModeEasy == AScreen::SME_UseWorkbench) ||
       (m_ScreenModeEasy == AScreen::SME_UseNamedPubScreen))
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

bool AScreen::IsOpen() const
{
  return (m_pScreen != NULL);
}

const char* AScreen::Title() const
{
  return m_Title.C_str();
}

void AScreen::SetTitle(SimpleString title)
{
  if(IsOpen())
  {
    // Screen is already open, so we don't change its title dynamically
    return;
  }

  m_Title = title;
}

UWORD AScreen::FontHeight() const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Font->tf_YSize;
}

WORD AScreen::BarHeight() const
{
  if(m_pScreen == NULL)
  {
    return 0;
  }

	return m_pScreen->WBorTop + FontHeight() + 2;
}

struct Screen* AScreen::IntuiScreen()
{
  return m_pScreen;
}

struct Window* AScreen::ActiveWindow() const
{
  // Locking the intuition base
  ULONG intuitionLock = LockIBase(0L);

  // Getting the active window
  struct Window* pActiveWin = IntuitionBase->ActiveWindow;

  // Unlocking  the intuition base
  UnlockIBase(intuitionLock);

  return pActiveWin;
}

struct DrawInfo* AScreen::IntuiDrawInfo()
{
  return m_pDrawInfo;
}

struct TextAttr* AScreen::IntuiTextAttr()
{
  if(m_pScreen == NULL)
  {
    return 0;
  }

  return m_pScreen->Font;
}

APTR* AScreen::GadtoolsVisualInfo()
{
  return m_pVisualInfo;
}

const ADiffViewPens& AScreen::Pens() const
{
  return m_Pens;
}

