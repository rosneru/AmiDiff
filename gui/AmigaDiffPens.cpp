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
  m_FirstColorNum = p_FirstFreeColorNum;

  struct ViewPort* pViewPort = &m_pAppScreen->IntuiScreen()->ViewPort;

  // Red for 'deleted'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 12, 16, 12);

  // Yellow for 'changed'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 16, 16, 12);

  // Green for 'added'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 15, 11, 12);

  // Another grey for the background of the line numbers
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 10, 10, 10);

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
