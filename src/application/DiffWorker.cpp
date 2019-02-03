#include "DiffEngine.h"
#include "DiffWorker.h"

DiffWorker::DiffWorker(SimpleString& p_LeftFilePath,
                                 SimpleString& p_RightFilePath,
                                 DiffWindow& p_DiffWindow,
                                 FilesWindow& p_FilesWindow,
                                 ProgressWindow& p_ProgressWindow,
                                 struct MsgPort* p_pProgressPort)
  : BackgroundWorker(p_pProgressPort),
    m_LeftFilePath(p_LeftFilePath),
    m_RightFilePath(p_RightFilePath),
    m_DiffWindow(p_DiffWindow),
    m_FilesWindow(p_FilesWindow),
    m_ProgressWindow(p_ProgressWindow),
    m_ProgressOffset(0),
    m_pLeftDiffDocument(NULL),
    m_pRightDiffDocument(NULL)
{
}

DiffWorker::~DiffWorker()
{
  disposeDocuments();
}

const SimpleString& DiffWorker::ErrorText() const
{
  return m_ErrorText;
}

const SimpleString& DiffWorker::ElapsedText() const
{
  return m_ElapsedText;
}

bool DiffWorker::Diff()
{
  if(m_LeftFilePath.Length() == 0)
  {
    m_ErrorText = "Left file name not set.";
    return false;
  }

  if(m_RightFilePath.Length() == 0)
  {
    m_ErrorText = "Right file name not set.";
    return false;
  }

  long timePreprocessLeft = 0;
  long timePreprocessRight = 0;
  long timeDiff = 0;
  long timeSummary = 0;

  m_ProgressWindow.Open();

    /**
   * Closes the window.
   */
  void Close();

  APTR pDisabledMenuItem = m_FilesWindow.DisabledMenuItem();
  m_FilesWindow.Close();

  m_StopWatch.Start();

  // Send progress notifications to here. (This is a BackgroundWorker
  // who sends them as messages to the event loop).
  m_LeftSrcPartition.SetProgressReporter(this);

  setProgressDescription("Pre-processing left file..");

  if(m_LeftSrcPartition.PreProcess(m_LeftFilePath) == false)
  {
    m_ErrorText = "Error while pre-processing the file '";
    m_ErrorText += m_LeftFilePath + "'\n Maybe a read error?";

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    return false;
  }

  timePreprocessLeft = static_cast<long>(m_StopWatch.Stop());

  m_StopWatch.Start();

  // Send progress notifications to here. (This is a BackgroundWorker
  // who sends them as messages to the event loop).
  m_RightSrcPartition.SetProgressReporter(this);

  setProgressDescription("Pre-processing right file..");

  if(m_RightSrcPartition.PreProcess(m_RightFilePath) == false)
  {
    m_ErrorText = "Error while pre-processing the file \n'";
    m_ErrorText += m_RightFilePath + "'\n Maybe a read error?";

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    return false;
  }

  timePreprocessRight = static_cast<long>(m_StopWatch.Stop());

  m_StopWatch.Start();

  DiffEngine diffEngine;

  // Send progress notifications to here. (This is a BackgroundWorker
  // who sends them as messages to the event loop).
  diffEngine.SetProgressReporter(this);

  setProgressDescription("Comparing the files..");

  bool diffOk = diffEngine.Diff(
    m_LeftSrcPartition, m_RightSrcPartition,
    m_LeftDiffPartition, m_RightDiffPartition);

  timeDiff = static_cast<long>(m_StopWatch.Stop());

  timeSummary = timePreprocessLeft + timePreprocessRight + timeDiff;

  if(!diffOk)
  {
    m_ErrorText = "Error while performing the diff.";

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

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

  m_pLeftDiffDocument->SetFileName(m_LeftFilePath);
  m_pRightDiffDocument->SetFileName(m_RightFilePath);

  m_DiffWindow.Open();
  m_ProgressWindow.Close();

  m_DiffWindow.SetContent(m_pLeftDiffDocument, m_pRightDiffDocument);
  m_DiffWindow.SetStatusBarText(m_ElapsedText);

  return true;
}

void DiffWorker::disposeDocuments()
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

void DiffWorker::doWork()
{
  Diff();
}

void DiffWorker::notifyProgressChanged(int p_Progress)
{
  //
  // Reporting the 3 stages of diff-progress (preprocessing left file,
  // preprocessing right file, performing the diff as 0..33%, 33%..66%
  // and 66%..100%.
  //

  if(p_Progress == 100)
  {
    if(m_ProgressOffset == 0)
    {
      m_ProgressOffset = 33;
      p_Progress = 0;
    }
    else if(m_ProgressOffset == 33)
    {
      m_ProgressOffset = 66;
      p_Progress = 0;
    }
    else if(m_ProgressOffset == 66)
    {
      m_ProgressOffset = 0;
      p_Progress = -1;
    }
  }
  else
  {
    p_Progress = p_Progress / 3 + 1;
  }

  BackgroundWorker::notifyProgressChanged(m_ProgressOffset + p_Progress);
}
