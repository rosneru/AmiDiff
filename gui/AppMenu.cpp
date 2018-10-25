#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

#include "AppMenu.h"


AppMenu::AppMenu(AppScreen* p_pScreen)
  : m_pScreen(p_pScreen),
    m_pMenu(NULL)
{

}

AppMenu::~AppMenu()
{
  Dispose();
}

bool AppMenu::Create(struct NewMenu* p_pMenuDefinition)
{
  if(m_pScreen == NULL || m_pScreen->GadtoolsVisualInfo() == NULL)
  {
    // Without VisualInfo the menu can't be created
    return false;
  }

  // Menu building step 1: Create the menu
  m_pMenu = CreateMenus(p_pMenuDefinition, TAG_END);
  if(m_pMenu == NULL)
  {
    // Menu creation failed
    return false;
  }

  // Menu building step 2: Outlaying the menu
  if(LayoutMenus(m_pMenu, m_pScreen->GadtoolsVisualInfo(),
                 GTMN_NewLookMenus, TRUE, // Ignored before v39
                 TAG_END) == FALSE)
  {
    // Outlaying the menu failed
    Dispose();
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
}

bool AppMenu::AttachToWindow(struct Window* p_pWindow)
{
  if(SetMenuStrip(p_pWindow, m_pMenu) == FALSE)
  {
    // Binding the menu to given window failed
    return false;
  }

  return true;
}

bool AppMenu::UpdateInWindow(struct Window* p_pWindow)
{
  if(ResetMenuStrip(p_pWindow, m_pMenu) == FALSE)
  {
    // Binding the menu to given window failed
    return false;
  }

  return true;
}

void AppMenu::DetachFromWindow(struct Window* p_pWindow)
{
  ClearMenuStrip(p_pWindow);
}

struct Menu* AppMenu::IntuiMenu()
{
  return m_pMenu;
}
