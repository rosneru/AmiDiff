#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include "BackgroundWorker.h"
#include "WorkerProgressMsg.h"


BackgroundWorker::BackgroundWorker(struct MsgPort* p_pProgressPort)
  : m_pStartupMsg(NULL),
    m_pProgressPort(p_pProgressPort),
    m_pReplyPort(NULL),
    m_pProgressDescription(NULL),
    m_pBackgrProcess(NULL),
    m_bExitRequested(false)
{
}

BackgroundWorker::~BackgroundWorker()
{
  workDone();
}

bool BackgroundWorker::Run()
{
  if(m_pBackgrProcess != NULL)
  {
    // Do not start the process if it's already running
    return false;
  }

  // Create the background process
  m_pBackgrProcess = CreateNewProcTags(
    NP_Name, "BackgroundWorker",
    NP_Entry, &startup,
    TAG_END);

  if(m_pBackgrProcess == NULL)
  {
    return false;
  }

  m_pStartupMsg = (struct WorkerStartupMsg*) AllocVec(
    sizeof(struct WorkerStartupMsg), MEMF_CLEAR|MEMF_PUBLIC);

  if(m_pStartupMsg == NULL)
  {
    return false;
  }

  m_pStartupMsg->that = this;
  PutMsg(&m_pBackgrProcess->pr_MsgPort, m_pStartupMsg);

  return true;
}


void BackgroundWorker::startup()
{
  struct Process* proc = (struct Process *)FindTask(NULL);

  WaitPort(&proc->pr_MsgPort);

  struct WorkerStartupMsg* pStartupMsg = (struct WorkerStartupMsg*)
    GetMsg(&proc->pr_MsgPort);

  if((pStartupMsg == NULL) || (pStartupMsg->that == NULL))
  {
    return;
  }

  class BackgroundWorker* that = pStartupMsg->that;

  // Create the reply port for this process. It is used to receive
  // answers from main for the sent progress messages.
  pStartupMsg->that->m_pReplyPort = CreateMsgPort();
  if (that->m_pReplyPort == NULL)
  {
    return;
  }

  that->doWork();

  DeleteMsgPort(that->m_pReplyPort);
  // TODO Maybe activate this
  //that->m_pBackgrProcess = NULL;
}

void BackgroundWorker::workDone()
{
  if(m_pStartupMsg != NULL)
  {
    FreeVec(m_pStartupMsg);
    m_pStartupMsg = NULL;
  }

  // TODO Is this enough?
  m_pBackgrProcess = NULL;
}

void BackgroundWorker::setProgressDescription(const char* p_pProgressDescription)
{
  m_pProgressDescription = p_pProgressDescription;
}


void BackgroundWorker::notifyProgressChanged(int p_Progress)
{
  if((m_pProgressPort == NULL) || (m_pReplyPort == NULL))
  {
    return;
  }

  // Creating and initializing the progress message
  struct WorkerProgressMsg progressMessage;
  progressMessage.mn_ReplyPort = m_pReplyPort;
  progressMessage.progress = p_Progress;
  progressMessage.pDescription = m_pProgressDescription;

  // Sending the progress message, waiting for the answer and taking the
  // answer from the queue
  PutMsg(m_pProgressPort, &progressMessage);
  WaitPort(m_pReplyPort);
  GetMsg(m_pReplyPort);
}
