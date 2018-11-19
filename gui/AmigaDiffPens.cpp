#include <clib/graphics_protos.h>
#include <exec/libraries.h>

#include "AppScreen.h"
#include "AmigaDiffPens.h"

//extern struct Library* GfxBase;

AmigaDiffPens::AmigaDiffPens()
  : m_pAppScreen(NULL),
    m_bInitialized(false)
{

}

AmigaDiffPens::~AmigaDiffPens()
{
  Dispose();
}

bool AmigaDiffPens::Init(AppScreen* p_pAppScreen, short p_FirstFreeColorNum)
{
  if(m_bInitialized)
  {
    return false;
  }

  if(p_pAppScreen == NULL)
  {
    return false;
  }
/*
  if(p_FirstFreeColorNum < 4 || p_FirstFreeColorNum > 31)
  {
    return false;
  }
*/
  m_pAppScreen = p_pAppScreen;
/*
  if(GfxBase->lib_Version < 39)
  {
    // We need to use the OptainBestPen function from OS3.0 and above.
    // So we leave here, if this requirement is not given.
    return;
  }
*/
  if(!m_pAppScreen->IsOpen())
  {
    return false;
  }

  struct ColorMap* pColorMap = m_pAppScreen->IntuiScreen()->ViewPort.ColorMap;
  if(pColorMap == NULL)
  {
    return false;
  }

  m_RedPen = ObtainBestPen(pColorMap,
    0xf3f3f3f3, 0xb5b5b5b5, 0xb9b9b9b9,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_YellowPen = ObtainBestPen(pColorMap,
    0xfcfcfcfc, 0xffffffff, 0xbbbbbbbb,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_GreenPen = ObtainBestPen(pColorMap,
    0xc1c1c1c1, 0xfefefefe, 0xbdbdbdbd,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_GreyPen = ObtainBestPen(pColorMap,
    0x28282828, 0x28282828, 0x28282828,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_bInitialized = true;

  // struct ViewPort* pViewPort = &m_pAppScreen->IntuiScreen()->ViewPort;

  // // Red for 'deleted'
  // m_RedColorNum = p_FirstFreeColorNum++;
  // SetRGB4(pViewPort, m_RedColorNum, 12, 16, 12);

  // // Yellow for 'changed'
  // m_YellowColorNum = p_FirstFreeColorNum++;
  // SetRGB4(pViewPort, m_YellowColorNum, 16, 16, 12);

  // // Green for 'added'
  // m_GreenColorNum = p_FirstFreeColorNum++;
  // SetRGB4(pViewPort, m_GreenColorNum, 15, 11, 12);

  // // Another grey for the background of the line numbers
  // m_GreyColorNum = p_FirstFreeColorNum++;
  // SetRGB4(pViewPort, m_GreyColorNum, 10, 10, 10);

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

  ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_RedPen);
  ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_YellowPen);
  ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_GreenPen);
  ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_GreyPen);
  m_bInitialized = false;
}

LONG AmigaDiffPens::Background() const
{
  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];
}


LONG AmigaDiffPens::Text()  const
{
  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[TEXTPEN];
}


LONG AmigaDiffPens::HighlightedText()  const
{
  if(m_pAppScreen == NULL || m_pAppScreen->IntuiDrawInfo() == NULL)
  {
    return 3;
  }

  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[HIGHLIGHTTEXTPEN];
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
