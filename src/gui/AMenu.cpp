#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

#include "AMenu.h"


AMenu::AMenu()
  : m_pMenu(NULL)
{

}


AMenu::~AMenu()
{
  Dispose();
}


bool AMenu::Create(struct NewMenu* pMenuDefinition, ScreenBase* pScreen)
{
  if(pScreen == NULL || !pScreen->IsOpen())
  {
    return false;
  }

  if(pScreen->GadtoolsVisualInfo() == NULL)
  {
    // No VisualInfo available to create the menu
    return false;
  }

  // Create the menu
  m_pMenu = CreateMenus(pMenuDefinition, TAG_DONE);
  if(m_pMenu == NULL)
  {
    // Failed to create the menu
    return false;
  }

  // Create the layout for the menu
  if(LayoutMenus(m_pMenu, pScreen->GadtoolsVisualInfo(),
                 GTMN_NewLookMenus, TRUE, // Ignored before v39
                 TAG_DONE) == FALSE)
  {
    // Failed to create the layout for the menu
    Dispose();
    return false;
  }

  return true;
}


void AMenu::Dispose()
{
  if(m_pMenu != NULL)
  {
    FreeMenus(m_pMenu);
    m_pMenu = NULL;
  }
}


bool AMenu::AttachToWindow(struct Window* pWindow)
{
  if(SetMenuStrip(pWindow, m_pMenu) == FALSE)
  {
    // Binding the menu to given window failed
    return false;
  }

  return true;
}




void AMenu::DetachFromWindow(struct Window* pWindow)
{
  ClearMenuStrip(pWindow);
}

struct Menu* AMenu::IntuiMenu()
{
  return m_pMenu;
}


void AMenu::DisableMenuItem(struct Window* pWindow,
  APTR pUserDataMenuItemToDisable)
{
  if(pUserDataMenuItemToDisable == NULL)
  {
    return;
  }

  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData(pUserDataMenuItemToDisable, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OffMenu(pWindow, menuNumber);
}


void AMenu::EnableMenuItem(struct Window* pWindow,
  APTR pUserDataMenuItemToEnable)
{
  if(pUserDataMenuItemToEnable == NULL)
  {
    return;
  }

  WORD menuNumber = 0;
  struct MenuItem* pFoundItem = findItemByUserData(pUserDataMenuItemToEnable, menuNumber);
  if(pFoundItem == NULL)
  {
    return;
  }

  OnMenu(pWindow, menuNumber);
}


struct MenuItem* AMenu::findItemByUserData(APTR pUserDataToFind, 
                                             WORD& foundMenuNumber)
{
  if(m_pMenu == NULL)
  {
    return NULL;
  }

  foundMenuNumber = 0;

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
      if(pUserData == pUserDataToFind)
      {
        foundMenuNumber = FULLMENUNUM(iMenu, iItem, 0);
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
