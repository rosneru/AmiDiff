#include <clib/intuition_protos.h>
#include "CommandBase.h"


CommandBase::CommandBase(Array<WindowBase*>& windowArray)
  : m_Windows(windowArray),
    m_SleepRequester()
{

}

CommandBase::~CommandBase()
{

}


void CommandBase::EnableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    struct Window* pIntuiWindow = m_Windows[i]->IntuiWindow();
    AMenu* pMenu = m_Windows[i]->Menu();

    if(pIntuiWindow != NULL && pMenu != NULL)
    {
      pMenu->EnableMenuItem(m_Windows[i]->IntuiWindow(), (APTR)this);
    }
  }
}


void CommandBase::DisableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    struct Window* pIntuiWindow = m_Windows[i]->IntuiWindow();
    AMenu* pMenu = m_Windows[i]->Menu();

    if(pIntuiWindow != NULL && pMenu != NULL)
    {
      pMenu->DisableMenuItem(m_Windows[i]->IntuiWindow(), (APTR)this);
    }
  }
}


void CommandBase::disableBusyPointerForAllWindows()
{
  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    struct Window* pIntuiWindow = m_Windows[i]->IntuiWindow();

    if(pIntuiWindow != NULL && m_Windows[i]->IsOpen())
    {
      EndRequest(&m_SleepRequester, pIntuiWindow);
      SetWindowPointer(pIntuiWindow, WA_BusyPointer, FALSE, TAG_DONE);
    }
  }
}

void CommandBase::enableBusyPointerForAllWindows()
{  
  InitRequester(&m_SleepRequester);

  for(size_t i = 0; i < m_Windows.Size(); i++)
  {
    struct Window* pIntuiWindow = m_Windows[i]->IntuiWindow();

    if(pIntuiWindow != NULL)
    {
      Request(&m_SleepRequester, pIntuiWindow);
      SetWindowPointer(pIntuiWindow, WA_BusyPointer, TRUE, TAG_DONE);
    }
  }
}
