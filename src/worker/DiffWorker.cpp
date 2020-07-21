#include <clib/exec_protos.h>

#include "MessageBox.h"
#include "DiffEngineAmiga.h"
#include "DiffWorker.h"

DiffWorker::DiffWorker(SimpleString& leftFilePath,
                       SimpleString& rightFilePath,
                       DiffWindow& diffWindow,
                       ProgressWindow& progressWindow,
                       CmdOpenWindow& cmdOpenFilesWindow,
                       CmdCloseWindow& cmdCloseFilesWindow,
                       struct MsgPort*& pProgressPort,
                       bool& bCancelRequested,
                       bool& bExitAllowed)
  : WorkerBase(pProgressPort),
    m_LeftSrcFilePath(leftFilePath),
    m_RightSrcFilePath(rightFilePath),
    m_DiffWindow(diffWindow),
    m_ProgressWindow(progressWindow),
    m_CmdOpenFilesWindow(cmdOpenFilesWindow),
    m_CmdCloseFilesWindow(cmdCloseFilesWindow),
    m_ProgressOffset(0),
    m_bCancelRequested(bCancelRequested),
    m_bExitAllowed(bExitAllowed),
    m_pPoolHeader(NULL),
    m_pDiffDocument(NULL),
    m_pLeftSrcFile(NULL),
    m_pRightSrcFile(NULL),
    m_pLeftDiffFile(NULL),
    m_pRightDiffFile(NULL),
    m_bShowLineNumbers(false)
{

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
  m_CmdCloseFilesWindow.Execute(NULL);
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

    m_CmdOpenFilesWindow.Execute(NULL);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }


  //
  // Some basic tests
  //
  if(m_LeftSrcFilePath.Length() == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Left file name not set.",
                 "Ok");

    m_CmdOpenFilesWindow.Execute(NULL);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  if(m_RightSrcFilePath.Length() == 0)
  {
    request.Show(m_ProgressWindow.IntuiWindow(),
                 "Error: Right file name not set.",
                 "Ok");

    m_CmdOpenFilesWindow.Execute(NULL);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  m_StopWatch.Start();

  try
  {
    // Load and analyze the left file
    setProgressDescription("Loading left file");
    m_pLeftSrcFile = new DiffInputFileAmiga(m_pPoolHeader,
                                            m_bCancelRequested,
                                            this,
                                            m_LeftSrcFilePath.C_str());

    // Load and analyze the right file
    setProgressDescription("Loading right file");
    m_pRightSrcFile = new DiffInputFileAmiga(m_pPoolHeader,
                                             m_bCancelRequested,
                                             this,
                                             m_RightSrcFilePath.C_str());

    // Collect line numbers if this is enabled
    if(m_bShowLineNumbers)
    {
      size_t maxNumLines = m_pLeftSrcFile->NumLines();
      if(m_pRightSrcFile->NumLines() > maxNumLines)
      {
        maxNumLines = m_pRightSrcFile->NumLines();
      }

      m_pLeftSrcFile->CollectLineNumbers(maxNumLines);
      m_pRightSrcFile->CollectLineNumbers(maxNumLines);
    }

    m_pLeftDiffFile = new DiffOutputFileAmiga(m_pPoolHeader,
                                              m_bCancelRequested,
                                              this);

    m_pRightDiffFile = new DiffOutputFileAmiga(m_pPoolHeader,
                                               m_bCancelRequested,
                                               this);

    // Compare the files
    std::vector<size_t> m_DiffIndices;  // TODO remove, this will crash! Only to make building posible.
    setProgressDescription("Comparing the files");
    DiffEngine* pDiffEngine = new DiffEngine(m_pLeftSrcFile,
                                             m_pRightSrcFile,
                                             m_pLeftDiffFile,
                                             m_pRightDiffFile,
                                             m_bCancelRequested,
                                             m_DiffIndices);
    pDiffEngine->SetProgressReporter(this);
    bool diffOk = pDiffEngine->Diff();

    // If there was an error return to FilesWindow
    if(!diffOk)
    {
      throw "Error while performing the diff.";
    }

    long totalTime = m_StopWatch.Pick();

    // If there are no changes return to FilesWindow
    if(pDiffEngine->NumDifferences() == 0)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                  "No differences found: the files are equal.", "Ok");

      m_CmdOpenFilesWindow.Execute(NULL);
      m_ProgressWindow.Close();

      m_bExitAllowed = true;
      return false;
    }

    //
    // Prepare diff window and set results
    //
    m_pDiffDocument = new DiffDocument(*m_pLeftDiffFile, 
                                      m_LeftSrcFilePath.C_str(),
                                      *m_pRightDiffFile,
                                      m_RightSrcFilePath.C_str());

    m_DiffWindow.SetLineNumbersVisible(m_bShowLineNumbers);
    m_DiffWindow.Open(WindowBase::IP_Fill);
    m_DiffWindow.SetContent(m_pDiffDocument,
                            totalTime,
                            pDiffEngine->NumAdded(),
                            pDiffEngine->NumChanged(),
                            pDiffEngine->NumDeleted());

    m_ProgressWindow.Close();

  }
  catch(const char* pError)
  {
    if(!m_bCancelRequested)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                   pError, "Abort");
    }

    m_CmdOpenFilesWindow.Execute(NULL);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

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


  if(m_pLeftSrcFile != NULL)
  {
    delete m_pLeftSrcFile;;
    m_pLeftSrcFile = NULL;
  }

  if(m_pRightSrcFile != NULL)
  {
    delete m_pRightSrcFile;;
    m_pRightSrcFile = NULL;
  }

  if(m_pLeftDiffFile != NULL)
  {
    delete m_pLeftDiffFile;;
    m_pLeftDiffFile = NULL;
  }

  if(m_pRightDiffFile != NULL)
  {
    delete m_pRightDiffFile;;
    m_pRightDiffFile = NULL;
  }

  // m_DiffIndices.clear();

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
