#include <clib/intuition_protos.h>
#include "AppScreen.h"


AppScreen::AppScreen(SimpleString p_Title)
  : m_Title(p_Title),
    m_pScreen(NULL)
{
}

AppScreen::~AppScreen()
{
  Close();
}

bool AppScreen::Open()
{
  //
  // Initial validations
  //

  if(m_pScreen != NULL)
  {
    // Not opening the screen if it is already open
    return false;
  }

  //
  // Opening the screen
  //

  m_pScreen = OpenScreenTags(
    NULL,
    SA_LikeWorkbench, TRUE,
    SA_Title, m_Title.C_str(),
    TAG_DONE);

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

  return true;
}

void AppScreen::Close()
{
  if(m_pDrawInfo != NULL)
  {
    FreeScreenDrawInfo(m_pScreen, m_pDrawInfo);
    m_pDrawInfo = NULL;
  }

  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }
}

const char* AppScreen::Title()
{
  return m_Title.C_str();
}


struct Screen* AppScreen::IntuiScreen()
{
  return m_pScreen;
}

struct DrawInfo* AppScreen::IntuiDrawInfo()
{
  return m_pDrawInfo;
}
