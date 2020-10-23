#ifdef __clang__
  #include <clib/gadtools_protos.h>
  #include <clib/graphics_protos.h>
  #include <clib/intuition_protos.h>
#else
  #include <proto/gadtools.h>
  #include <proto/graphics.h>
  #include <proto/intuition.h>
#endif

#include <graphics/text.h>
#include <intuition/intuitionbase.h>

#include "OpenScreenBase.h"

extern struct IntuitionBase* IntuitionBase;

ScreenBase::ScreenBase(const ADiffViewSettings& settings)
  : m_pIntuiScreen(NULL),
    m_pDrawInfo(NULL),
    m_Settings(settings),
    m_NumWindowsOpen(0),
    m_pVisualInfo(NULL)
{
}

ScreenBase::~ScreenBase()
{

}


struct Screen* ScreenBase::IntuiScreen()
{
  return m_pIntuiScreen;
}

struct DrawInfo* ScreenBase::IntuiDrawInfo()
{
  return m_pDrawInfo;
}

struct TextAttr* ScreenBase::IntuiTextAttr()
{
  if(m_pIntuiScreen == NULL)
  {
    return 0;
  }

  return m_pIntuiScreen->Font;
}

APTR ScreenBase::GadtoolsVisualInfo()
{
  return m_pVisualInfo;
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

void ScreenBase::ToFront() const
{
  ScreenToFront(m_pIntuiScreen);
}

bool ScreenBase::IsHiresMode() const
{
  if(m_pIntuiScreen == NULL)
  {
    return false;
  }

  if(m_pIntuiScreen->Flags & SCREENHIRES)
  {
    return true;
  }

  return false;
}
