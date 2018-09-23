#include <clib/intuition_protos.h>
#include "AppScreen.h"


AppScreen::AppScreen()
{
  m_pScreen = NULL;
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
    SA_Title, "AmiDiff (C) 2018 by Uwe Rosner.",
    TAG_DONE);

  return m_pScreen != NULL;
}

void AppScreen::Close()
{
  if(m_pScreen != NULL)
  {
    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }
}

struct Screen* AppScreen::Screen()
{
  return m_pScreen;
}