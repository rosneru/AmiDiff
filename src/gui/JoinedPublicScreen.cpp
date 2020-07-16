#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

#include "JoinedPublicScreen.h"

JoinedPublicScreen::JoinedPublicScreen(const ADiffViewSettings& settings,
                                       const char* pPubScreenName)
  : ScreenBase(settings),
    m_PubScreenName(pPubScreenName)
{

}

JoinedPublicScreen::~JoinedPublicScreen()
{
  Close();
}

bool JoinedPublicScreen::Open()
{
  m_pIntuiScreen = LockPubScreen(m_PubScreenName.C_str());

  if(m_pIntuiScreen == NULL)
  {
    return false;
  }

  m_pDrawInfo = GetScreenDrawInfo(m_pIntuiScreen);
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
  m_pVisualInfo = (APTR) GetVisualInfo(m_pIntuiScreen, TAG_DONE);
  if(m_pVisualInfo == NULL)
  {
    Close();
    return false;
  }

  return true;
}

void JoinedPublicScreen::Close()
{
  if(m_pVisualInfo != NULL)
  {
    FreeVisualInfo(m_pVisualInfo);
    m_pVisualInfo = NULL;
  }

  // Freeing the allocated pens before closing the screen as the intui
  // screen is needed there
  m_Pens.Dispose();

  if(m_pDrawInfo != NULL && m_pIntuiScreen != NULL)
  {
    FreeScreenDrawInfo(m_pIntuiScreen, m_pDrawInfo);
    m_pDrawInfo = NULL;
  }

  if(m_pIntuiScreen != NULL)
  {
    UnlockPubScreen(NULL, m_pIntuiScreen);
    m_pIntuiScreen = NULL;
  }
}
