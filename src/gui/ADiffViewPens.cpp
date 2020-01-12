#include <clib/graphics_protos.h>

#include "ADiffViewPens.h"


ADiffViewPens::ADiffViewPens(struct Screen*& pScreen,
                             struct DrawInfo*& pDrawInfo,
                             ADiffViewSettings& settings)
  : m_pScreen(pScreen),
    m_pDrawInfo(pDrawInfo),
    m_Settings(settings),
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

  if(m_pScreen->ViewPort.ColorMap == NULL)
  {
    return false;
  }

  ULONG* pColRed = m_Settings.GetColorRedArray();
  ULONG* pColGreen = m_Settings.GetColorGreenArray();
  ULONG* pColYellow = m_Settings.GetColorYellowArray();
  ULONG* pColGray = m_Settings.GetColorGrayArray();

  // Find the best pens for the neded colors
  m_RedPen = ObtainBestPen(m_pScreen->ViewPort.ColorMap,
    pColRed[0], pColRed[1], pColRed[2],
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_DONE);

  m_GreenPen = ObtainBestPen(m_pScreen->ViewPort.ColorMap,
    pColGreen[0], pColGreen[1], pColGreen[2],
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_DONE);

  m_YellowPen = ObtainBestPen(m_pScreen->ViewPort.ColorMap,
    pColYellow[0], pColYellow[1], pColYellow[2],
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_DONE);

  m_GrayPen = ObtainBestPen(m_pScreen->ViewPort.ColorMap,
    pColGray[0], pColGray[1], pColGray[2],
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_EXACT,
    TAG_DONE);

  return true;
}


void ADiffViewPens::Dispose()
{
  if(m_pScreen == NULL)
  {
    return;
  }

  if(m_pScreen->ViewPort.ColorMap == NULL)
  {
    return;
  }

  ReleasePen(m_pScreen->ViewPort.ColorMap, m_RedPen);
  ReleasePen(m_pScreen->ViewPort.ColorMap, m_YellowPen);
  ReleasePen(m_pScreen->ViewPort.ColorMap, m_GreenPen);
  ReleasePen(m_pScreen->ViewPort.ColorMap, m_GrayPen);
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

