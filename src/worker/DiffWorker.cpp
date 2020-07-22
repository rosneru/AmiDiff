#include <clib/exec_protos.h>

#include "MessageBox.h"
#include "DiffWorker.h"

DiffWorker::DiffWorker(SimpleString& leftFilePath,
                       SimpleString& rightFilePath,
                       DiffWindow& diffWindow,
                       ProgressWindow& progressWindow,
                       CmdOpenWindow& cmdOpenFilesWindow,
                       CmdCloseWindow& cmdCloseFilesWindow,
                       struct MsgPort*& pProgressPort,
                       bool& bCancelRequested,
                       bool& bExitAllowed,
                       bool bShowLineNumbers)
  : m_LeftSrcFilePath(leftFilePath),
    m_RightSrcFilePath(rightFilePath),
    m_Progress(pProgressPort, m_pReplyPort),
    m_DiffWindow(diffWindow),
    m_ProgressWindow(progressWindow),
    m_CmdOpenFilesWindow(cmdOpenFilesWindow),
    m_CmdCloseFilesWindow(cmdCloseFilesWindow),
    m_bCancelRequested(bCancelRequested),
    m_bExitAllowed(bExitAllowed),
    m_bShowLineNumbers(bShowLineNumbers),
    m_pDiffDocument(NULL)
{
}

DiffWorker::~DiffWorker()
{
  if(m_pDiffDocument != NULL)
  {
    delete m_pDiffDocument;
    m_pDiffDocument = NULL;
  }
}


bool DiffWorker::Diff()
{
  MessageBox request;
  m_bCancelRequested = false;
  m_bExitAllowed = false;
  m_Progress.Reset();

  //
  // Close FilesWindow, open ProgressWindow etc
  //
  m_ProgressWindow.Open();
  m_CmdCloseFilesWindow.Execute(NULL);
  m_DiffWindow.Close();


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

  if(m_pDiffDocument != NULL)
  {
    delete m_pDiffDocument;
    m_pDiffDocument = NULL;
  }

  try
  {
    m_StopWatch.Start();
    m_pDiffDocument = new DiffDocument(m_LeftSrcFilePath.C_str(),
                                       m_RightSrcFilePath.C_str(),
                                       m_bCancelRequested,
                                       m_StopWatch,
                                       m_Progress,
                                       m_bShowLineNumbers);

    if(m_pDiffDocument->NumDifferences() < 1)
    {
      throw "No differences found: the files are equal.";
    }
  }
  catch(const char* pError)
  {
    if(!m_bCancelRequested)
    {
      request.Show(m_ProgressWindow.IntuiWindow(),
                   pError, "Ok.");
    }

    m_CmdOpenFilesWindow.Execute(NULL);
    m_ProgressWindow.Close();

    m_bExitAllowed = true;
    return false;
  }

  m_DiffWindow.Open(WindowBase::IP_Fill);
  m_DiffWindow.SetContent(m_pDiffDocument);

  m_ProgressWindow.Close();

  m_bExitAllowed = true;
  return true;
}


void DiffWorker::doWork()
{
  Diff();
}
