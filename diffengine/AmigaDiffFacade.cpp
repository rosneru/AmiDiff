#include "AmigaDiffFacade.h"

AmigaDiffFacade::AmigaDiffFacade(TextWindow& p_LeftWindow, 
  TextWindow& p_RightWindow)
  : m_LeftWindow(p_LeftWindow),
    m_RightWindow(p_RightWindow),
    m_pLeftFileDiff(NULL),
    m_pRightFileDiff(NULL)
{
}

void AmigaDiffFacade::SetLeftFilePath(const char* p_pLeftFilePath)
{
  m_LeftFilePath = SimpleString(p_pLeftFilePath);
}

const SimpleString& AmigaDiffFacade::LeftFilePath()
{
  return m_LeftFilePath;
}

void AmigaDiffFacade::SetRightFilePath(const char* p_pRightFilePath)
{
  m_RightFilePath = SimpleString(p_pRightFilePath);
}

const SimpleString& AmigaDiffFacade::RightFilePath()
{
  return m_RightFilePath;
}


bool AmigaDiffFacade::Diff()
{
  return false;
}

DiffFilePartition* AmigaDiffFacade::LeftFileDiff()
{
  return m_pLeftFileDiff;
}

DiffFilePartition* AmigaDiffFacade::RightFileDiff()
{
  return m_pRightFileDiff;
}
