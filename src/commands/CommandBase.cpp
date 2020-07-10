#include <clib/intuition_protos.h>
#include "CommandBase.h"

Array<WindowBase*>* CommandBase::m_pAllWindowsArray(NULL);

CommandBase::CommandBase(Array<WindowBase*>* pAllWindowsArray)
  : m_SleepRequester()
{
  m_pAllWindowsArray = pAllWindowsArray;
}

CommandBase::~CommandBase()
{

}


void CommandBase::EnableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_pAllWindowsArray->Size(); i++)
  {
    struct Window* pIntuiWindow = (*m_pAllWindowsArray)[i]->IntuiWindow();
    AMenu* pMenu = (*m_pAllWindowsArray)[i]->Menu();

    if(pIntuiWindow != NULL && pMenu != NULL)
    {
      pMenu->EnableMenuItem((*m_pAllWindowsArray)[i]->IntuiWindow(), (APTR)this);
    }
  }
}


void CommandBase::DisableInAllWindowMenus() const
{
  for(size_t i = 0; i < m_pAllWindowsArray->Size(); i++)
  {
    struct Window* pIntuiWindow = (*m_pAllWindowsArray)[i]->IntuiWindow();
    AMenu* pMenu = (*m_pAllWindowsArray)[i]->Menu();

    if(pIntuiWindow != NULL && pMenu != NULL)
    {
      pMenu->DisableMenuItem((*m_pAllWindowsArray)[i]->IntuiWindow(), (APTR)this);
    }
  }
}


void CommandBase::disableBusyPointerForAllWindows()
{
  for(size_t i = 0; i < m_pAllWindowsArray->Size(); i++)
  {
    struct Window* pIntuiWindow = (*m_pAllWindowsArray)[i]->IntuiWindow();

    if(pIntuiWindow != NULL && (*m_pAllWindowsArray)[i]->IsOpen())
    {
      EndRequest(&m_SleepRequester, pIntuiWindow);
      SetWindowPointer(pIntuiWindow, WA_BusyPointer, FALSE, TAG_DONE);
    }
  }
}

void CommandBase::enableBusyPointerForAllWindows()
{  
  InitRequester(&m_SleepRequester);

  for(size_t i = 0; i < m_pAllWindowsArray->Size(); i++)
  {
    struct Window* pIntuiWindow = (*m_pAllWindowsArray)[i]->IntuiWindow();

    if(pIntuiWindow != NULL)
    {
      Request(&m_SleepRequester, pIntuiWindow);
      SetWindowPointer(pIntuiWindow, WA_BusyPointer, TRUE, TAG_DONE);
    }
  }
}
