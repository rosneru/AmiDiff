#include <clib/graphics_protos.h>

#include "AppScreen.h"
#include "AmigaDiffPens.h"


AmigaDiffPens::AmigaDiffPens()
  : m_pAppScreen(NULL)
{

}

AmigaDiffPens::~AmigaDiffPens()
{

}

bool AmigaDiffPens::Init(AppScreen* p_pAppScreen, short p_FirstFreeColorNum)
{
  if(p_pAppScreen == NULL)
  {
    return false;
  }

  if(p_FirstFreeColorNum < 4 || p_FirstFreeColorNum > 31)
  {
    return false;
  }

  m_pAppScreen = p_pAppScreen;

  struct ViewPort* pViewPort = &m_pAppScreen->IntuiScreen()->ViewPort;

  // Red for 'deleted'
  m_RedColorNum = p_FirstFreeColorNum++;
  SetRGB4(pViewPort, m_RedColorNum, 12, 16, 12);

  // Yellow for 'changed'
  m_YellowColorNum = p_FirstFreeColorNum++;
  SetRGB4(pViewPort, m_YellowColorNum, 16, 16, 12);

  // Green for 'added'
  m_GreenColorNum = p_FirstFreeColorNum++;
  SetRGB4(pViewPort, m_GreenColorNum, 15, 11, 12);

  // Another grey for the background of the line numbers
  m_GreyColorNum = p_FirstFreeColorNum++;
  SetRGB4(pViewPort, m_GreyColorNum, 10, 10, 10);

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
    return m_RedColorNum;
  }

  ULONG AmigaDiffPens::Yellow() const
  {
    return m_YellowColorNum;
  }

  ULONG AmigaDiffPens::Green() const
  {
    return m_GreenColorNum;
  }

  ULONG AmigaDiffPens::Grey() const
  {
    return m_GreyColorNum;
  }
