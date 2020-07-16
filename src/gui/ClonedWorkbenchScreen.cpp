#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include "ClonedWorkbenchScreen.h"

ClonedWorkbenchScreen::ClonedWorkbenchScreen(const ADiffViewSettings& settings,
                                             const char* pTitle,
                                             short depth)
  : ScreenBase(settings),
    m_pTitle(pTitle),
    m_Depth(depth)
{

}

ClonedWorkbenchScreen::~ClonedWorkbenchScreen()
{
  Close();
}

bool ClonedWorkbenchScreen::Open()
{
  //
  // Opening a nearly-copy of the Workbench screen
  //
  // It is only *nearly* a copy because it'll have 8 colors instead
  // of how many the Workbench screen might have.
  //
  m_pIntuiScreen = OpenScreenTags(NULL,
                             SA_LikeWorkbench, TRUE,
                             SA_Type, CUSTOMSCREEN,
                             SA_Depth, m_Depth,
                             SA_SharePens,TRUE,
                             SA_Title, m_pTitle,
                             SA_Colors32, m_Settings.GetColorArray(),
                             TAG_DONE);

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


void ClonedWorkbenchScreen::Close()
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
    CloseScreen(m_pIntuiScreen);
    m_pIntuiScreen = NULL;
  }
}
