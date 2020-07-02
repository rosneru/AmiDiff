#include "CommandBase.h"


CommandBase::CommandBase(Array<WindowBase*>& windowArray)
  : m_WindowArray(windowArray)
{

}

CommandBase::~CommandBase()
{

}

void CommandBase::disableThisCmdInAllWindowsMenus()
{
  for(size_t i = 0; i < m_WindowArray.Size(); i++)
  {
    AMenu* pMenu = m_WindowArray[i]->Menu();
    if(pMenu != NULL)
    {
      pMenu->DisableMenuItem(m_WindowArray[i]->IntuiWindow(), (APTR)this);
    }
  }
}

void CommandBase::enableThisCmdInAllWindowsMenus()
{
  for(size_t i = 0; i < m_WindowArray.Size(); i++)
  {
    AMenu* pMenu = m_WindowArray[i]->Menu();
    if(pMenu != NULL)
    {
      pMenu->EnableMenuItem(m_WindowArray[i]->IntuiWindow(), (APTR)this);
    }
  }
}