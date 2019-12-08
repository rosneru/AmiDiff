#include <clib/graphics_protos.h>

#include "ADiffViewPens.h"


ADiffViewPens::ADiffViewPens(struct Screen*& pScreen,
                             struct DrawInfo*& pDrawInfo)
  : m_pScreen(pScreen),
    m_pDrawInfo(pDrawInfo),
    m_RedPen(-1),
    m_YellowPen(-1),
    m_GreenPen(-1),
    m_GrayPen(-1)
{

}

ADiffViewPens::~ADiffViewPens()
{
  Dispose();
}

bool ADiffViewPens::Create()
{
  if(m_pScreen == NULL)
  {
    return false;
  }

  m_pColorMap = m_pScreen->ViewPort.ColorMap;
  if(m_pColorMap == NULL)
  {
    return false;
  }

  // Find the best pens for the neded colors
  m_RedPen = ObtainBestPen(m_pColorMap,
    0xf3f3f3f3, 0xb5b5b5b5, 0xb9b9b9b9,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_YellowPen = ObtainBestPen(m_pColorMap,
    0xfcfcfcfc, 0xffffffff, 0xbbbbbbbb,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_GreenPen = ObtainBestPen(m_pColorMap,
    0xc1c1c1c1, 0xfefefefe, 0xbdbdbdbd,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  m_GrayPen = ObtainBestPen(m_pColorMap,
    0x28282828, 0x28282828, 0x28282828,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_END);

  return true;
}

void ADiffViewPens::Dispose()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  if(m_pColorMap == NULL)
  {
    return;
  }

  ReleasePen(m_pColorMap, m_RedPen);
  ReleasePen(m_pColorMap, m_YellowPen);
  ReleasePen(m_pColorMap, m_GreenPen);
  ReleasePen(m_pColorMap, m_GrayPen);
}

LONG ADiffViewPens::Background() const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Pens[BACKGROUNDPEN];
}


LONG ADiffViewPens::Text()  const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Pens[TEXTPEN];
}


LONG ADiffViewPens::HighlightedText()  const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Pens[HIGHLIGHTTEXTPEN];
}


LONG ADiffViewPens::Fill()  const
{
  if(m_pDrawInfo == NULL)
  {
    return 0;
  }

  return m_pDrawInfo->dri_Pens[FILLPEN];
}


LONG ADiffViewPens::Red() const
{
  if(m_RedPen < 0)
  {
    return 0;
  }

  return m_RedPen;
}


LONG ADiffViewPens::Yellow() const
{
  if(m_YellowPen < 0)
  {
    return 0;
  }

  return m_YellowPen;
}

LONG ADiffViewPens::Green() const
{
  if(m_GreenPen < 0)
  {
    return 0;
  }

  return m_GreenPen;
}

LONG ADiffViewPens::Gray() const
{
  if(m_GrayPen < 0)
  {
    return 0;
  }

  return m_GrayPen;
}

