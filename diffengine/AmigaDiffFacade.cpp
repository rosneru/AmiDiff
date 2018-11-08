#include "AmigaDiffFacade.h"

AmigaDiffFacade::AmigaDiffFacade()
  : m_pLeftFileDiff(NULL),
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

void AmigaDiffFacade::SetLeftDiffWindow(DiffWindow* p_pLeftWindow)
{
  m_pLeftWindow = p_pLeftWindow;
}

void AmigaDiffFacade::SetRightDiffWindow(DiffWindow* p_pRightWindow)
{
  m_pRightWindow = p_pRightWindow;
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
