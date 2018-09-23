#include <libraries/gadtools.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

#include "AppMenu.h"


AppMenu::AppMenu(struct Screen* p_pScreen)
{
  m_pMenu = NULL;

  // Get visual info from screen
  m_pVisualInfo = (APTR*)GetVisualInfo(p_pScreen, TAG_END);

}

AppMenu::~AppMenu()
{
  Dispose();
}

bool AppMenu::Create()
{
  if(m_pVisualInfo == NULL)
  {
    // Without VisualInfo the menu can't be created
    return false;
  }

  // Menu building step 1: Create the menu
  struct NewMenu appMenuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open left file...",     "L", 0, 0, 0 },
    {  NM_ITEM,   "Open right file...",    "R", 0, 0, 0 },
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, 0 },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  m_pMenu = CreateMenus(appMenuDefinition, TAG_END);
  if(m_pMenu == NULL)
  {
    // Menu creation failed
    return false;
  }

  // Menu building step 2: Layouting the menu
  if(LayoutMenus(m_pMenu, m_pVisualInfo,
                 GTMN_NewLookMenus, TRUE, // Ignored before v39
                 TAG_END) == FALSE)
  {
    // Layouting the menu failed
    FreeMenus(m_pMenu);
    m_pMenu = NULL;
    return false;
  }

  return true;
}

void AppMenu::Dispose()
{
  if(m_pMenu != NULL)
  {
    FreeMenus(m_pMenu);
    m_pMenu = NULL;
  }

  if(m_pVisualInfo != NULL)
  {
    FreeVisualInfo(m_pVisualInfo);
    m_pMenu = NULL;
  }
}

bool AppMenu::BindToWindow(struct Window* p_pWindow)
{
  if(SetMenuStrip(p_pWindow, m_pMenu) == FALSE)
  {
    // Binding the menu to given window failed
    return false;
  }

  return true;
}

struct Menu* AppMenu::Menu()
{
  return m_pMenu;
}
