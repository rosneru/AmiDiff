#include <clib/graphics_protos.h>
#include "AmigaDiffPens.h"


AmigaDiffPens::AmigaDiffPens(AppScreen& p_AppScreen)
  : m_AppScreen(p_AppScreen)
{

}

AmigaDiffPens::~AmigaDiffPens()
{

}

bool AmigaDiffPens::Init(short p_FirstFreeColorNum)
{
  if(p_FirstFreeColorNum < 4 || p_FirstFreeColorNum > 31)
  {
    return false;
  }

  m_FirstColorNum = p_FirstFreeColorNum;

  struct ViewPort* pViewPort = &m_AppScreen.IntuiScreen()->ViewPort;

  // Red for 'deleted'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 12, 16, 12);

  // Yellow for 'changed'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 16, 16, 12);

  // Green for 'added'
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 15, 11, 12);

  // Another grey for the background of the line numbers
  SetRGB4(pViewPort, p_FirstFreeColorNum++, 10, 10, 10);
}

ULONG AmigaDiffPens::Background() const
{
  return m_AppScreen.IntuiDrawInfo()->dri_Pens[BACKGROUNDPEN];
}

ULONG AmigaDiffPens::Text()  const
{
  return m_AppScreen.IntuiDrawInfo()->dri_Pens[TEXTPEN];
}
  
ULONG AmigaDiffPens::HighlightedText()  const
{
  return m_AppScreen.IntuiDrawInfo()->dri_Pens[HIGHLIGHTTEXTPEN];
}
  