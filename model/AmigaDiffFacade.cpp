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
  m_LeftFilePath = p_pLeftFilePath;
}

const SimpleString& AmigaDiffFacade::LeftFilePath() const
{
  return m_LeftFilePath;
}

void AmigaDiffFacade::SetRightFilePath(const char* p_pRightFilePath)
{
  m_RightFilePath = p_pRightFilePath;
}

const SimpleString& AmigaDiffFacade::RightFilePath() const
{
  return m_RightFilePath;
}

const SimpleString& AmigaDiffFacade::ErrorText() const
{
  return m_ErrorText;
}


bool AmigaDiffFacade::Diff()
{
  if(m_LeftFilePath.Length() == 0)
  {
    m_ErrorText = "Left file name not set.";
    return false;
  }

  if(m_RightFilePath.Length() == 0)
  {
    m_ErrorText = "Right file iname not set.";
    return false;
  }

  if(m_LeftSrcPartition.PreProcess(m_LeftFilePath) == false)
  {
    m_ErrorText = "Error in pre-processing the left file. Maybe a read/write error?";
    return false;
  }

  if(m_RightSrcPartition.PreProcess(m_RightFilePath) == false)
  {
    m_ErrorText = "Error in pre-processing the right file. Maybe a read/write error?";
    return false;
  }

  DiffEngine diffEngine;
  bool diffOk = diffEngine.Diff(
    m_LeftSrcPartition, m_RightSrcPartition,
    m_LeftDiffPartition, m_RightDiffPartition);

  if(!diffOk)
  {
    m_ErrorText = "Error while performing the diff.";
    return false;
  }

  m_ErrorText = "";

  disposeDocuments();

  m_pLeftDiffDocument = new DiffDocument(m_LeftDiffPartition);
  m_pRightDiffDocument = new DiffDocument(m_RightDiffPartition);

  m_pLeftDiffDocument->SetFileName(LeftFilePath());
  m_pRightDiffDocument->SetFileName(RightFilePath());

  m_LeftWindow.SetInitialPosition(WindowBase::IP_Left);
  m_LeftWindow.SetFixed(true);
  m_LeftWindow.Open();

  m_RightWindow.SetInitialPosition(WindowBase::IP_Right);
  m_RightWindow.SetFixed(true);
  m_RightWindow.Open();

  m_LeftWindow.SetContent(m_pLeftDiffDocument);
  m_RightWindow.SetContent(m_pRightDiffDocument);


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
