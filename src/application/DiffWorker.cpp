#include <clib/exec_protos.h>

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
    m_LeftSrcFilePath(leftFilePath),
    m_RightSrcFilePath(rightFilePath),
    m_DiffWindow(diffWindow),
    m_FilesWindow(filesWindow),
    m_ProgressWindow(progressWindow),
    m_ProgressOffset(0),
    m_bCancelRequested(bCancelRequested),
    m_bExitAllowed(bExitAllowed),
    m_pPoolHeader(NULL),
    m_pDiffDocumentLeft(NULL),
    m_pDiffDocumentRight(NULL),
    m_LeftSrcFile(m_pPoolHeader, m_bCancelRequested),
    m_RightSrcFile(m_pPoolHeader, m_bCancelRequested),
    m_LeftDiffFile(m_pPoolHeader, m_bCancelRequested),
    m_RightDiffFile(m_pPoolHeader, m_bCancelRequested),
    m_DiffEngine(m_LeftSrcFile,
                 m_RightSrcFile,
                 m_LeftDiffFile,
                 m_RightDiffFile,
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
  m_LeftSrcFile.SetProgressReporter(this);
  m_RightSrcFile.SetProgressReporter(this);
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
  if(m_LeftSrcFilePath.Length() == 0)
  {
    request.Show("Error: Left file name not set.", "Ok");

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  if(m_RightSrcFilePath.Length() == 0)
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

  m_pPoolHeader = CreatePool(MEMF_CLEAR, 50000, 25000);
  if(m_pPoolHeader == NULL)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Failed to create the memory pool.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  m_StopWatch.Start();

  //
  // Pre-processing the left file
  //
  setProgressDescription("Pre-processing left file..");

  // If there was an error return to FilesWindow
  if(m_LeftSrcFile.PreProcess(m_LeftSrcFilePath.C_str()) == false)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 m_LeftSrcFile.Error(), "Ok");

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
  if(m_RightSrcFile.PreProcess(m_RightSrcFilePath.C_str()) == false)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 m_RightSrcFile.Error(), "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    workDone();
    m_bExitAllowed = true;
    return false;
  }

  //
  // Compare the files
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
  m_LeftDiffFile.NumChanges(leftNumAdded, leftNumChanged,
                            leftNumDeleted);

  int rightNumAdded;
  int rightNumChanged;
  int rightNumDeleted;
  m_RightDiffFile.NumChanges(rightNumAdded, rightNumChanged,
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

  m_pDiffDocumentLeft = new DiffDocument(m_LeftDiffFile);
  m_pDiffDocumentRight = new DiffDocument(m_RightDiffFile);

  m_pDiffDocumentLeft->SetFileName(m_LeftSrcFilePath.C_str());
  m_pDiffDocumentRight->SetFileName(m_RightSrcFilePath.C_str());

  m_DiffWindow.Open();
  m_ProgressWindow.Close();

  m_DiffWindow.SetContent(m_pDiffDocumentLeft, m_pDiffDocumentRight);
  m_DiffWindow.SetStatusBar(timeSummary, leftNumAdded + rightNumAdded,
                            leftNumChanged + rightNumChanged,
                            leftNumDeleted + rightNumDeleted);

  workDone();
  m_bExitAllowed = true;
  return true;
}

void DiffWorker::disposeDocuments()
{
  if(m_pDiffDocumentLeft != NULL)
  {
    delete m_pDiffDocumentLeft;
    m_pDiffDocumentLeft = NULL;
  }

  if(m_pDiffDocumentRight != NULL)
  {
    delete m_pDiffDocumentRight;
    m_pDiffDocumentRight = NULL;
  }

  m_LeftSrcFile.Clear();
  m_RightSrcFile.Clear();
  m_LeftDiffFile.Clear();
  m_RightDiffFile.Clear();

  // Deleting the memory pool as a whole gives an extreme
  // performence boost at exit or when starting another compare.
  DeletePool(m_pPoolHeader);
  m_pPoolHeader = NULL;
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
