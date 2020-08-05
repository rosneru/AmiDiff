#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

#include "OpenJoinedPublicScreen.h"

OpenJoinedPublicScreen::OpenJoinedPublicScreen(const ADiffViewSettings& settings,
                                               const char* pPubScreenName)
  : ScreenBase(settings),
    m_PubScreenName(pPubScreenName)
{
  m_pIntuiScreen = LockPubScreen(m_PubScreenName.c_str());

  if(m_pIntuiScreen == NULL)
  {
    throw "Failed to lock public screen.";
  }

  m_pDrawInfo = GetScreenDrawInfo(m_pIntuiScreen);
  if(m_pDrawInfo == NULL)
  {
    CloseScreen(m_pIntuiScreen);
    throw "Failed to get the screen draw info";
  }

  // Trying to initialize our four needed color pens
  if(m_Pens.Create() == false)
  {
    FreeScreenDrawInfo(m_pIntuiScreen, m_pDrawInfo);
    CloseScreen(m_pIntuiScreen);
    throw "Failed to the create pens.";
  }

  // Get visual info from screen
  m_pVisualInfo = (APTR) GetVisualInfo(m_pIntuiScreen, TAG_DONE);
  if(m_pVisualInfo == NULL)
  {
    m_Pens.Dispose();
    FreeScreenDrawInfo(m_pIntuiScreen, m_pDrawInfo);
    CloseScreen(m_pIntuiScreen);
    throw "Failed to get the visual info.";
  }
}

OpenJoinedPublicScreen::~OpenJoinedPublicScreen()
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
