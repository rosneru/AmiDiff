#include <proto/graphics.h>

#include "AppScreen.h"
#include "AmigaDiffPens.h"


AmigaDiffPens::AmigaDiffPens()
  : m_pAppScreen(NULL),
    m_bInitialized(false),
    m_bObtainedPens(false)
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

  switch(m_pAppScreen->ScreenMode())
  {
    case AppScreen::SME_CloneWorkbenchMin8Col:
    {
      // Case 1 - We use a Workbench screen clone with at least
      // 8 colors, so we try to set pens 4..7 to our needed values

      // Starting with color number 4 as 0..3 are system reserved
      int colorNum = 4;

      // Red for 'deleted'
      m_RedPen = colorNum++;
      SetRGB4(&m_pAppScreen->IntuiScreen()->ViewPort,
        m_RedPen, 14, 11, 11);

      // Yellow for 'changed'
      m_YellowPen = colorNum++;
      SetRGB4(&m_pAppScreen->IntuiScreen()->ViewPort,
        m_YellowPen, 15, 15, 11);

      // Green for 'added'
      m_GreenPen = colorNum++;
      SetRGB4(&m_pAppScreen->IntuiScreen()->ViewPort,
        m_GreenPen, 12, 15, 12);

      // Another grey for the background of the line numbers
      m_GreyPen = colorNum++;
      SetRGB4(&m_pAppScreen->IntuiScreen()->ViewPort,
        m_GreyPen, 10, 10, 10);

      break;
    }
    case AppScreen::SME_UseWorkbench:
    case AppScreen::SME_UseNamedPubScreen:
    {
      // Case 2 - We use the Workbench or another public screen, so we
      // try to use pen sharing to get our needed pens
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

      m_bObtainedPens = true;

      break;
    }
    default:
    {
      // All other screenmodes / cases: Setting all our special colors
      // to pen 3 (blue in most cases)
      m_RedPen = 3;
      m_YellowPen = 3;
      m_GreenPen = 3;
      m_GreyPen = 3;
      break;
    }
  }

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


  if(m_bObtainedPens == true)
  {
    struct ColorMap* pColorMap =
      m_pAppScreen->IntuiScreen()->ViewPort.ColorMap;

    ReleasePen(pColorMap, m_RedPen);
    ReleasePen(pColorMap, m_YellowPen);
    ReleasePen(pColorMap, m_GreenPen);
    ReleasePen(pColorMap, m_GreyPen);
  }

  m_bObtainedPens = false;
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
