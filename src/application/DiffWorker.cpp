#include "MessageBox.h"
#include "DiffWorker.h"

DiffWorker::DiffWorker(SimpleString& leftFilePath,
                       SimpleString& rightFilePath,
                       DiffWindow& diffWindow,
                       FilesWindow& filesWindow,
                       ProgressWindow& progressWindow,
                       struct MsgPort* pProgressPort,
                       bool& bCancelRequested,
                       bool& bExitAllowed)
  : BackgroundWorker(pProgressPort),
    m_LeftFilePath(leftFilePath),
    m_RightFilePath(rightFilePath),
    m_DiffWindow(diffWindow),
    m_FilesWindow(filesWindow),
    m_ProgressWindow(progressWindow),
    m_ProgressOffset(0),
    m_bCancelRequested(bCancelRequested),
    m_bExitAllowed(bExitAllowed),
    m_pLeftDiffDocument(NULL),
    m_pRightDiffDocument(NULL),
    m_LeftSrcPartition(m_bCancelRequested),
    m_RightSrcPartition(m_bCancelRequested),
    m_LeftDiffPartition(m_bCancelRequested),
    m_RightDiffPartition(m_bCancelRequested),
    m_DiffEngine(m_LeftSrcPartition,
                 m_RightSrcPartition,
                 m_LeftDiffPartition,
                 m_RightDiffPartition,
                 m_bCancelRequested)
{

  //
  // Registering *this* as receiver for progress messages for some
  // objects.
  //
  // NOTE *this* is a BackgroundWorker who forwards these messages to
  //      the intuition event loop which eventually displays the
  //      progress in a window.
  //
  m_LeftSrcPartition.SetProgressReporter(this);
  m_RightSrcPartition.SetProgressReporter(this);
  m_DiffEngine.SetProgressReporter(this);
}

DiffWorker::~DiffWorker()
{
  disposeDocuments();
}

bool DiffWorker::Diff()
{
  MessageBox request;
  m_bCancelRequested = false;
  m_bExitAllowed = false;
  m_ProgressOffset = 0;

  //
  // Checking some obvious things
  //
  if(m_LeftFilePath.Length() == 0)
  {
    request.Show("Error: Left file name not set.", "Ok");

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  if(m_RightFilePath.Length() == 0)
  {
    request.Show("Error: Right file name not set.", "Ok");

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Closing FilesWindow, opening ProgressWindow etc
  //
  m_ProgressWindow.Open();
  APTR pDisabledMenuItem = m_FilesWindow.DisabledMenuItem();
  m_FilesWindow.Close();
  m_DiffWindow.Close();
  disposeDocuments();
  m_StopWatch.Start();

  //
  // Pre-processing the left file
  //
  setProgressDescription("Pre-processing left file..");

  // If there was an error return to FilesWindow
  if(m_LeftSrcPartition.PreProcess(m_LeftFilePath.C_str()) == false)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Can't read left file.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Pre-processing the right file
  //
  setProgressDescription("Pre-processing right file..");

  // If there was an error return to FilesWindow
  if(m_RightSrcPartition.PreProcess(m_RightFilePath.C_str()) == false)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Can't read right file.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Comparing the files
  //
  setProgressDescription("Comparing the files..");
  bool diffOk = m_DiffEngine.Diff();
  long timeSummary = static_cast<long>(m_StopWatch.Pick());

  // If there was an error return to FilesWindow
  if(!diffOk)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error while performing the diff.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Collecting the number of changes
  //
  int leftNumAdded;
  int leftNumChanged;
  int leftNumDeleted;
  m_LeftDiffPartition.NumChanges(leftNumAdded, leftNumChanged,
                                 leftNumDeleted);

  int rightNumAdded;
  int rightNumChanged;
  int rightNumDeleted;
  m_RightDiffPartition.NumChanges(rightNumAdded, rightNumChanged,
                                  rightNumDeleted);

  // If there are no changes return to FilesWindow
  if((leftNumAdded + leftNumChanged + leftNumDeleted +
     rightNumAdded + rightNumChanged + rightNumDeleted) == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "No differences found: the files are equal.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Prepare diff window and set results
  //

  m_pLeftDiffDocument = new DiffDocument(m_LeftDiffPartition);
  m_pRightDiffDocument = new DiffDocument(m_RightDiffPartition);

  m_pLeftDiffDocument->SetFileName(m_LeftFilePath.C_str());
  m_pRightDiffDocument->SetFileName(m_RightFilePath.C_str());

  m_DiffWindow.Open();
  m_ProgressWindow.Close();

  m_DiffWindow.SetContent(m_pLeftDiffDocument, m_pRightDiffDocument);
  m_DiffWindow.SetStatusBar(timeSummary, leftNumAdded + rightNumAdded,
                            leftNumChanged + rightNumChanged,
                            leftNumDeleted + rightNumDeleted);

  workDone();
  m_bExitAllowed = true;
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

  m_LeftSrcPartition.Clear();
  m_RightSrcPartition.Clear();
  m_LeftDiffPartition.Clear();
  m_RightDiffPartition.Clear();
}

void DiffWorker::doWork()
{
  Diff();
}

void DiffWorker::notifyProgressChanged(int progress)
{
  //
  // Reporting the 3 stages of diff-progress (preprocessing left file,
  // preprocessing right file, performing the diff as 0..33%, 33%..66%
  // and 66%..100%.
  //

  if(progress == 100)
  {
    if(m_ProgressOffset == 0)
    {
      m_ProgressOffset = 33;
      progress = 0;
    }
    else if(m_ProgressOffset == 33)
    {
      m_ProgressOffset = 66;
      progress = 0;
    }
    else if(m_ProgressOffset == 66)
    {
      m_ProgressOffset = 0;
      progress = -1;
    }
  }
  else
  {
    progress = progress / 3 + 1;
  }

  BackgroundWorker::notifyProgressChanged(m_ProgressOffset + progress);
}
