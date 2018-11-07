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


void AppMenu::DisableMenuItem(struct Window* p_pWindow, 
  APTR p_pUserDataMenuItemToDisable)
{
  if(p_pUserDataMenuItemToDisable == NULL)
  {
    return;
  }

  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData(p_pUserDataMenuItemToDisable, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OffMenu(p_pWindow, menuNumber);
}

void AppMenu::EnableMenuItem(struct Window* p_pWindow, 
  APTR p_pUserDataMenuItemToEnable)
{
  if(p_pUserDataMenuItemToEnable == NULL)
  {
    return;
  }
  
  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData(p_pUserDataMenuItemToEnable, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OnMenu(p_pWindow, menuNumber);
}

struct MenuItem* AppMenu::findItemByUserData(APTR p_pUserDataToFind, WORD& p_FoundMenuNumber)
{
  if(m_pMenu == NULL)
  {
    return NULL;
  }

  p_FoundMenuNumber = 0;

  struct Menu* pMenu = m_pMenu;
  struct MenuItem* pItem = pMenu->FirstItem;
  if(pItem == NULL)
  {
    return NULL;
  }

  int iMenu = 0;
  int iItem = 0;

  do
  {
    do
    {
      APTR pUserData = GTMENUITEM_USERDATA(pItem);
      if(pUserData == p_pUserDataToFind)
      {
        p_FoundMenuNumber = FULLMENUNUM(iMenu, iItem, 0);
        return pItem;
      }

      pItem = pItem->NextItem;
      iItem++; 
    }
    while(pItem != NULL);

    pMenu = pMenu->NextMenu;
    if(pMenu != NULL)
    {
      pItem = pMenu->FirstItem;
      iItem = 0;
      iMenu++;
    }
  }
  while(pItem != NULL);

  return NULL;
}