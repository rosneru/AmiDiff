#include <clib/graphics_protos.h>
#include <exec/libraries.h>

#include "AppScreen.h"
#include "AmigaDiffPens.h"

extern struct Library* GfxBase;

AmigaDiffPens::AmigaDiffPens()
  : m_pAppScreen(NULL),
    m_bInitialized(false)
{

}

AmigaDiffPens::~AmigaDiffPens()
{
  if(m_bInitialized)
  {
    ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_RedPen);
    ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_YellowPen);
    ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_GreenPen);
    ReleasePen(m_pAppScreen->IntuiScreen()->ViewPort.ColorMap, m_GreyPen);
  }
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

  if(p_FirstFreeColorNum < 4 || p_FirstFreeColorNum > 31)
  {
    return false;
  }

  m_pAppScreen = p_pAppScreen;

  if(GfxBase->lib_Version < 39)
  {
    // We need to use the OptainBestPen function from OS3.0 and above. 
    // So we leave here, if this requirement is not given.
    return;
  }

  m_RedPen = ObtainBestPen(
    m_pAppScreen->IntuiScreen()->ViewPort.ColorMap,
    243, 181, 185,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_YellowPen = ObtainBestPen(
    m_pAppScreen->IntuiScreen()->ViewPort.ColorMap,
    252, 255, 187,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_GreenPen = ObtainBestPen(
    m_pAppScreen->IntuiScreen()->ViewPort.ColorMap,
    193, 254, 189,
    OBP_FailIfBad, FALSE,
    OBP_Precision, PRECISION_GUI,
    TAG_END);

  m_GreyPen = ObtainBestPen(
    m_pAppScreen->IntuiScreen()->ViewPort.ColorMap,
    40, 40, 40,
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

ULONG AmigaDiffPens::Background() const
{
  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];
}

ULONG AmigaDiffPens::Text()  const
{
  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[TEXTPEN];
}

ULONG AmigaDiffPens::HighlightedText()  const
{
  return m_pAppScreen->IntuiDrawInfo()->dri_Pens[HIGHLIGHTTEXTPEN];
}

  ULONG AmigaDiffPens::Red() const
  {
    return m_RedPen;
  }

  ULONG AmigaDiffPens::Yellow() const
  {
    return m_YellowPen;
  }

  ULONG AmigaDiffPens::Green() const
  {
    return m_GreenPen;
  }

  ULONG AmigaDiffPens::Grey() const
  {
    return m_GreyPen;
  }
