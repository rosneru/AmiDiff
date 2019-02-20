#include <clib/graphics_protos.h>

#include "AppScreen.h"
#include "AmigaDiffPens.h"


AmigaDiffPens::AmigaDiffPens()
  : m_pAppScreen(NULL),
    m_bInitialized(false),
    m_RedPen(0),
    m_YellowPen(0),
    m_GreenPen(0),
    m_GreyPen(0)
{

}

AmigaDiffPens::~AmigaDiffPens()
{
  Dispose();
}

bool AmigaDiffPens::Init(AppScreen* p_pAppScreen)
{
  if(m_bInitialized)
  {
    return false;
  }

  if(p_pAppScreen == NULL)
  {
    return false;
  }

  if(!p_pAppScreen->IsOpen())
  {
    return false;
  }

  m_pAppScreen = p_pAppScreen;
  struct ColorMap* pColorMap = m_pAppScreen->IntuiScreen()->ViewPort.ColorMap;
  if(pColorMap == NULL)
  {
    return false;
  }

  // Find the best pens for the neded colors
  m_RedPen = ObtainBestPen(pColorMap,
    0xf3f3f3f3, 0xb5b5b5b5, 0xb9b9b9b9,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_YellowPen = ObtainBestPen(pColorMap,
    0xfcfcfcfc, 0xffffffff, 0xbbbbbbbb,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_GreenPen = ObtainBestPen(pColorMap,
    0xc1c1c1c1, 0xfefefefe, 0xbdbdbdbd,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_GreyPen = ObtainBestPen(pColorMap,
    0x28282828, 0x28282828, 0x28282828,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_bInitialized = true;
  return true;
}

void AmigaDiffPens::Dispose()
{
  if(!m_bInitialized)
  {
    return;
  }


  if(m_pAppScreen == NULL || !m_pAppScreen->IsOpen())
  {
    return;
  }

  struct ColorMap* pColorMap =
    m_pAppScreen->IntuiScreen()->ViewPort.ColorMap;

  ReleasePen(pColorMap, m_RedPen);
  ReleasePen(pColorMap, m_YellowPen);
  ReleasePen(pColorMap, m_GreenPen);
  ReleasePen(pColorMap, m_GreyPen);

  m_bInitialized = false;
}

LONG AmigaDiffPens::Background() const
{
  if(!m_bInitialized)
  {
    return 3;
  }

  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];
}


LONG AmigaDiffPens::Text()  const
{
  if(!m_bInitialized)
  {
    return 3;
  }

  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[TEXTPEN];
}


LONG AmigaDiffPens::HighlightedText()  const
{
  if(!m_bInitialized)
  {
    return 3;
  }

  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[HIGHLIGHTTEXTPEN];
}

LONG AmigaDiffPens::Fill()  const
{
  if(!m_bInitialized)
  {
    return 3;
  }

  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[FILLPEN];
}


LONG AmigaDiffPens::Red() const
{
  return m_RedPen;
}

LONG AmigaDiffPens::Yellow() const
{
  return m_YellowPen;
}

LONG AmigaDiffPens::Green() const
{
  return m_GreenPen;
}

LONG AmigaDiffPens::Grey() const
{
  return m_GreyPen;
}
