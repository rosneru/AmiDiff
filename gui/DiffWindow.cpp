#include <clib/intuition_protos.h>
#include "DiffWindow.h"


DiffWindow::DiffWindow(const struct Screen* p_pScreen)
{
  m_pScreen = p_pScreen;
  m_pWindow = NULL;
}

DiffWindow::~DiffWindow()
{
  Close();
}

bool DiffWindow::Open(DW_TYPE p_DwType)
{
  //
  // Initial validations
  //

  if(m_pScreen == NULL)
  {
    // In this design we need a screen to open the window
    return false;
  }

  if(m_pWindow != NULL)
  {
    // Not opening the window if it is already open
    return false;
  }

  //
  // Calculating window size in depency of screen dimesions
  //
  int screenWidth = m_pScreen->Width;
  int screenHeight = m_pScreen->Height;
  int screenBarHeight = m_pScreen->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  //
  // Setting up the window properties
  //

  struct TagItem windowTags[] =
  {
    { WA_Left, 0 },
    { WA_Top, screenBarHeight + 1 },
    { WA_Width, winWidth },
    { WA_Height, winHeight },
    { WA_Title, (ULONG)"Left Diff Window" },
    { WA_Activate, TRUE },
    { WA_PubScreen, (ULONG)m_pScreen },
    { WA_IDCMP, IDCMP_MENUPICK },
    { WA_NewLookMenus, TRUE },  // Ignored before v39
    { TAG_DONE, NULL },
  };

  //
  // Opening the window
  //
  m_pWindow = OpenWindowTagList(NULL, windowTags);

  return m_pWindow != NULL;
}

void DiffWindow::Close()
{
  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }
}

struct Window* DiffWindow::Window()
{
  return m_pWindow;
}
