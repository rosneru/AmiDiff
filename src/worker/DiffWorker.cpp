#include <clib/exec_protos.h>

#include "MessageBox.h"
#include "DiffWorker.h"
#include "LinkedListAmiga.h"

DiffWorker::DiffWorker(SimpleString& leftFilePath,
                       SimpleString& rightFilePath,
                       DiffWindow& diffWindow,
                       FilesWindow& filesWindow,
                       ProgressWindow& progressWindow,
                       struct MsgPort*& pProgressPort,
                       bool& bCancelRequested,
                       bool& bExitAllowed)
  : WorkerBase(pProgressPort),
    m_LeftSrcFilePath(leftFilePath),
    m_RightSrcFilePath(rightFilePath),
    m_DiffWindow(diffWindow),
    m_FilesWindow(filesWindow),
    m_ProgressWindow(progressWindow),
    m_ProgressOffset(0),
    m_bCancelRequested(bCancelRequested),
    m_bExitAllowed(bExitAllowed),
    m_pPoolHeader(NULL),
    m_DiffStartIdxsList(m_pPoolHeader),
    m_pDiffDocument(NULL),
    m_LeftSrcFile(m_pPoolHeader, m_bCancelRequested),
    m_RightSrcFile(m_pPoolHeader, m_bCancelRequested),
    m_LeftDiffFile(m_pPoolHeader, m_bCancelRequested),
    m_RightDiffFile(m_pPoolHeader, m_bCancelRequested),
    m_DiffEngine(m_LeftSrcFile,
                 m_RightSrcFile,
                 m_LeftDiffFile,
                 m_RightDiffFile,
                 m_pPoolHeader,
                 m_bCancelRequested,
                 &m_DiffStartIdxsList),
    m_bShowLineNumbers(false)
{

  //
  // Registering *this* as receiver for progress messages for some
  // objects.
  //
  // NOTE *this* is a WorkerBase who itself is a ProgressReporter.
  //      That one finally forwards these messages to the intuition
  //      event loop where they are eventually processed in a window.
  //
  m_LeftSrcFile.SetProgressReporter(this);
  m_RightSrcFile.SetProgressReporter(this);
  m_DiffEngine.SetProgressReporter(this);
}

DiffWorker::~DiffWorker()
{
  disposeDocuments();
}

void DiffWorker::SetLineNumbers(bool bEnabled)
{
  m_bShowLineNumbers = bEnabled;
}

bool DiffWorker::Diff()
{
  MessageBox request;
  m_bCancelRequested = false;
  m_bExitAllowed = false;
  m_ProgressOffset = 0;

  //
  // Close FilesWindow, open ProgressWindow etc
  //
  m_ProgressWindow.Open();
  APTR pDisabledMenuItem = m_FilesWindow.DisabledMenuItem();
  m_FilesWindow.Close();
  m_DiffWindow.Close();

  // Close documents if already existing.
  // Also deletes the memory pool if already allocated
  disposeDocuments();

  //
  // Create the memory pool
  //
  m_pPoolHeader = CreatePool(MEMF_ANY|MEMF_CLEAR, 50000, 25000);
  if(m_pPoolHeader == NULL)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Failed to create the memory pool.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }


  //
  // Checking some obvious things
  //
  if(m_LeftSrcFilePath.Length() == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Left file name not set.",
                 "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  if(m_RightSrcFilePath.Length() == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Right file name not set.",
                 "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

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
    if(!m_bCancelRequested)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                   m_LeftSrcFile.Error(), "Ok");
    }

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

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
    if(!m_bCancelRequested)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                   m_RightSrcFile.Error(), "Ok");
    }

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  // Collect line numbers if  this is enabled
  if(m_bShowLineNumbers)
  {
    size_t maxNumLines = m_LeftSrcFile.NumLines();
    if(m_RightSrcFile.NumLines() > maxNumLines)
    {
      maxNumLines = m_RightSrcFile.NumLines();
    }

    m_LeftSrcFile.CollectLineNumbers(maxNumLines);
    m_RightSrcFile.CollectLineNumbers(maxNumLines);
  }

  //
  // Compare the files
  //
  setProgressDescription("Comparing the files..");
  bool diffOk = m_DiffEngine.Diff();
  long totalTime = m_StopWatch.Pick();

  // If there was an error return to FilesWindow
  if(!diffOk)
  {
    if(!m_bCancelRequested)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                   "Error while performing the diff.", "Ok");
    }

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  // If there are no changes return to FilesWindow
  if(m_DiffEngine.NumDifferences() == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "No differences found: the files are equal.", "Ok");

    m_FilesWindow.Open(pDisabledMenuItem);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  //
  // Prepare diff window and set results
  //
  m_pDiffDocument = new DiffDocument(m_LeftDiffFile, 
                                     m_LeftSrcFilePath.C_str(),
                                     m_RightDiffFile,
                                     m_RightSrcFilePath.C_str());

  m_DiffWindow.SetLineNumbersVisible(m_bShowLineNumbers);
  m_DiffWindow.Open(NULL, WindowBase::IP_Fill);
  m_DiffWindow.SetContent(m_pDiffDocument,
                          &m_DiffStartIdxsList,
                          totalTime,
                          m_DiffEngine.NumAdded(),
                          m_DiffEngine.NumChanged(),
                          m_DiffEngine.NumDeleted());

  m_ProgressWindow.Close();


  m_bExitAllowed = true;
  return true;
}

void DiffWorker::disposeDocuments()
{
  if(m_pDiffDocument != NULL)
  {
    delete m_pDiffDocument;
    m_pDiffDocument = NULL;
  }

  m_LeftSrcFile.Clear();
  m_RightSrcFile.Clear();
  m_LeftDiffFile.Clear();
  m_RightDiffFile.Clear();

  m_DiffStartIdxsList.Clear();

  //
  // Deleting the memory pool as a whole gives an extreme
  // performence boost at exit or when starting another compare.
  //
  if(m_pPoolHeader != NULL)
  {
    DeletePool(m_pPoolHeader);
    m_pPoolHeader = NULL;
  }
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

  WorkerBase::notifyProgressChanged(m_ProgressOffset + progress);
}
