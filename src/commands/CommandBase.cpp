#include "CommandBase.h"


CommandBase::CommandBase(Array<WindowBase*>& windowArray)
  : m_Windows(windowArray)
{

}

CommandBase::~CommandBase()
{

}

void CommandBase::DisableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    AMenu* pMenu = m_Windows[i]->Menu();
    if(pMenu != NULL)
    {
      pMenu->DisableMenuItem(m_Windows[i]->IntuiWindow(), (APTR)this);
    }
  }
}

void CommandBase::EnableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    AMenu* pMenu = m_Windows[i]->Menu();
    if((pMenu != NULL) && (m_Windows[i]->IntuiWindow() != NULL))
    {
      pMenu->EnableMenuItem(m_Windows[i]->IntuiWindow(), (APTR)this);
    }
  }
}
