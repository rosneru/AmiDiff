#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <graphics/text.h>
#include <intuition/intuitionbase.h>
#include "ScreenBase.h"

extern struct IntuitionBase* IntuitionBase;

ScreenBase::ScreenBase(const ADiffViewSettings& settings)
  : m_pScreen(NULL),
    m_pDrawInfo(NULL),
    m_Pens(m_pScreen, m_pDrawInfo, settings),
    m_Settings(settings),
    m_NumWindowsOpen(0),
    m_pVisualInfo(NULL)
{
}

ScreenBase::~ScreenBase()
{

}


bool ScreenBase::IsOpen() const
{
  return (m_pScreen != NULL);
}

UWORD ScreenBase::FontHeight() const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Font->tf_YSize;
}

WORD ScreenBase::BarHeight() const
{
  if(m_pScreen == NULL)
  {
    return 0;
  }

	return m_pScreen->WBorTop + FontHeight() + 2;
}

struct Screen* ScreenBase::IntuiScreen()
{
  return m_pScreen;
}

struct DrawInfo* ScreenBase::IntuiDrawInfo()
{
  return m_pDrawInfo;
}

struct TextAttr* ScreenBase::IntuiTextAttr()
{
  if(m_pScreen == NULL)
  {
    return 0;
  }

  return m_pScreen->Font;
}

APTR ScreenBase::GadtoolsVisualInfo()
{
  return m_pVisualInfo;
}

const ADiffViewPens& ScreenBase::Pens() const
{
  return m_Pens;
}

size_t ScreenBase::NumOpenWindows() const
{
  return m_NumWindowsOpen;
}

void ScreenBase::IncreaseNumOpenWindows()
{
  m_NumWindowsOpen++;
}

void ScreenBase::DecreaseNumOpenWindows()
{
  if(m_NumWindowsOpen > 0)
  {
    m_NumWindowsOpen--;
  }
}

bool ScreenBase::IsHiresMode() const
{
  if(m_pScreen == NULL)
  {
    return false;
  }

  LONG modeId = GetVPModeID(&(m_pScreen->ViewPort));
  if(modeId == INVALID_ID)
  {
    return false;
  }

  DisplayInfoHandle pHandle = FindDisplayInfo(modeId);
  if(pHandle == NULL)
  {
    return false;
  }

  MonitorInfo monitorInfo;
  ULONG result = GetDisplayInfoData(pHandle, 
                                    &monitorInfo, 
                                    sizeof(monitorInfo),
                                    DTAG_MNTR,
                                    modeId);

  if(result == 0)
  {
    return false;
  }

  if(monitorInfo.Mspc->ratioh != monitorInfo.Mspc->ratiov)
  {
    return false;
  }

  return true;
}
