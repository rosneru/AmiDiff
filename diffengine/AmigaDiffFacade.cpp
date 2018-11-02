#include "AmigaDiffFacade.h"

AmigaDiffFacade::AmigaDiffFacade()
{
}

void AmigaDiffFacade::SetLeftFilePath(const char* p_pLeftFilePath)
{
  m_LeftFilePath = SimpleString(p_pLeftFilePath);
}

SimpleString AmigaDiffFacade::LeftFilePath()
{
  return m_LeftFilePath;
}

void AmigaDiffFacade::SetRightFilePath(const char* p_pRightFilePath)
{
  m_RightFilePath = SimpleString(p_pRightFilePath);
}

SimpleString AmigaDiffFacade::RightFilePath()
{
  return m_RightFilePath;
}
