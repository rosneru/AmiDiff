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

struct Window* ScreenBase::ActiveWindow() const
{
  // Locking the intuition base
  ULONG intuitionLock = LockIBase(0L);

  // Getting the active window
  struct Window* pActiveWin = IntuitionBase->ActiveWindow;

  // Unlocking  the intuition base
  UnlockIBase(intuitionLock);

  return pActiveWin;
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
