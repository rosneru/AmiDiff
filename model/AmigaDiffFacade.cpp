#include "DiffEngine.h"
#include "AmigaDiffFacade.h"

AmigaDiffFacade::AmigaDiffFacade(DiffWindow& p_DiffWindow)
  : m_DiffWindow(p_DiffWindow),
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

const SimpleString& AmigaDiffFacade::ElapsedText() const
{
  return m_ElapsedText;
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

  long timePreprocessLeft = 0;
  long timePreprocessRight = 0;
  long timeDiff = 0;
  long timeSummary = 0;

  m_StopWatch.Start();
  if(m_LeftSrcPartition.PreProcess(m_LeftFilePath) == false)
  {
    m_ErrorText = "Error while pre-processing the file '";
    m_ErrorText += m_LeftFilePath + "'\n Maybe a read error?";

    return false;
  }
  timePreprocessLeft = static_cast<long>(m_StopWatch.Stop());

  m_StopWatch.Start();
  if(m_RightSrcPartition.PreProcess(m_RightFilePath) == false)
  {
    m_ErrorText = "Error while pre-processing the file \n'";
    m_ErrorText += m_RightFilePath + "'\n Maybe a read error?";
    return false;
  }
  timePreprocessRight = static_cast<long>(m_StopWatch.Stop());

  m_StopWatch.Start();
  DiffEngine diffEngine;
  bool diffOk = diffEngine.Diff(
    m_LeftSrcPartition, m_RightSrcPartition,
    m_LeftDiffPartition, m_RightDiffPartition);

  timeDiff = static_cast<long>(m_StopWatch.Stop());

  timeSummary = timePreprocessLeft + timePreprocessRight + timeDiff;

  if(!diffOk)
  {
    m_ErrorText = "Error while performing the diff.";
    return false;
  }

  m_ErrorText = "";

  m_ElapsedText = "Diff performed in ";
  m_ElapsedText += timeSummary;
  m_ElapsedText += " ms. ( ";
  m_ElapsedText += timePreprocessLeft;
  m_ElapsedText += " + ";
  m_ElapsedText += timePreprocessRight;
  m_ElapsedText += " + ";
  m_ElapsedText += timeDiff;
  m_ElapsedText += ")";

  disposeDocuments();

  m_pLeftDiffDocument = new DiffDocument(m_LeftDiffPartition);
  m_pRightDiffDocument = new DiffDocument(m_RightDiffPartition);

  m_pLeftDiffDocument->SetFileName(LeftFilePath());
  m_pRightDiffDocument->SetFileName(RightFilePath());

  m_DiffWindow.Open();

  m_DiffWindow.SetContent(m_pLeftDiffDocument, m_pRightDiffDocument);
  m_DiffWindow.SetStatusBarText(m_ElapsedText);

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
