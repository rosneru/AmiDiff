#include "DiffEngine.h"
#include "AmigaDiffFacade.h"

AmigaDiffFacade::AmigaDiffFacade(TextWindow& p_LeftWindow, 
  TextWindow& p_RightWindow)
  : m_LeftWindow(p_LeftWindow),
    m_RightWindow(p_RightWindow),
    m_pLeftDiffDocument(NULL),
    m_pRightDiffDocument(NULL)
{
}

AmigaDiffFacade::~AmigaDiffFacade()
{
  disposeDocuments();
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
  if(m_LeftFilePath.Length() == 0 ||
     m_RightFilePath.Length() == 0)
  {
    return false;
  }

  if(m_LeftSrcPartition.PreProcess(m_LeftFilePath) == false)
  {
    return false;
  }

  if(m_RightSrcPartition.PreProcess(m_RightFilePath) == false)
  {
    return false;
  }

  DiffEngine diffEngine;
  bool diffOk = diffEngine.Diff( 
    m_LeftSrcPartition, m_RightSrcPartition,
    m_LeftDiffPartition, m_RightDiffPartition);
  
  if(!diffOk)
  {
    return false;
  }

  disposeDocuments();

  m_pLeftDiffDocument = new DiffDocument(m_LeftDiffPartition);
  m_pRightDiffDocument = new DiffDocument(m_RightDiffPartition);

  m_LeftWindow.SetContent(m_pLeftDiffDocument);
  m_RightWindow.SetContent(m_pRightDiffDocument);

  m_LeftWindow.Open(NULL, WindowBase::IWP_Left);
  m_LeftWindow.Open(NULL, WindowBase::IWP_Right);

  return true;
}

void AmigaDiffFacade::disposeDocuments()
{
  if(m_pLeftDiffDocument != NULL)
  {
    delete m_pLeftDiffDocument;
    m_pLeftDiffDocument = NULL;
  }

  if(m_pRightDiffDocument != NULL)
  {
    delete m_pRightDiffDocument;
    m_pRightDiffDocument = NULL;
  }
}
