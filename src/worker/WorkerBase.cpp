#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

#include "WorkerBase.h"
#include "ProgressMessage.h"

WorkerBase::WorkerBase(struct MsgPort*& pProgressPort)
  : m_pStartupMsg(NULL),
    m_pProgressPort(pProgressPort),
    m_pReplyPort(NULL),
    m_pProgressDescription(NULL),
    m_pBackgrProcess(NULL),
    m_bExitRequested(false)
{
}


WorkerBase::~WorkerBase()
{
  workDone();
}


bool WorkerBase::Run()
{
  if (m_pBackgrProcess != NULL)
  {
    // Do not start the process if it's already running
    return false;
  }

  // Create the background process
  m_pBackgrProcess = CreateNewProcTags(NP_Name, "WorkerBase",
                                       NP_Entry, &startup,
                                       TAG_END);

  if (m_pBackgrProcess == NULL)
  {
    return false;
  }

  m_pStartupMsg = (struct WorkerStartupMsg *)
    AllocVec(sizeof(struct WorkerStartupMsg), MEMF_CLEAR|MEMF_PUBLIC);

  if (m_pStartupMsg == NULL)
  {
    return false;
  }

  m_pStartupMsg->that = this;
  PutMsg(&m_pBackgrProcess->pr_MsgPort, m_pStartupMsg);

  return true;
}


void WorkerBase::startup()
{
  struct Process *pProcess = (struct Process *)FindTask(NULL);

  WaitPort(&pProcess->pr_MsgPort);

  struct WorkerStartupMsg *pStartupMsg = (struct WorkerStartupMsg *)
      GetMsg(&pProcess->pr_MsgPort);

  if ((pStartupMsg == NULL) || (pStartupMsg->that == NULL))
  {
    return;
  }

  class WorkerBase *that = pStartupMsg->that;

  // Create the reply port for this process. It is used to receive
  // answers from main for the sent progress messages.
  pStartupMsg->that->m_pReplyPort = CreateMsgPort();
  if (that->m_pReplyPort == NULL)
  {
    return;
  }

  that->doWork();

  DeleteMsgPort(that->m_pReplyPort);
}


void WorkerBase::workDone()
{
  if (m_pStartupMsg != NULL)
  {
    FreeVec(m_pStartupMsg);
    m_pStartupMsg = NULL;
  }

  // TODO Is this enough?
  m_pBackgrProcess = NULL;
}


void WorkerBase::setProgressDescription(const char *pProgressDescription)
{
  m_pProgressDescription = pProgressDescription;
}


void WorkerBase::notifyProgressChanged(int progress)
{
  if ((m_pProgressPort == NULL) || (m_pReplyPort == NULL))
  {
    return;
  }

  // Creating and initializing the progress message
  struct ProgressMessage progressMessage;
  progressMessage.mn_ReplyPort = m_pReplyPort;
  progressMessage.progress = progress;
  progressMessage.pDescription = m_pProgressDescription;

  // Sending the progress message, waiting for the answer and taking the
  // answer from the queue
  PutMsg(m_pProgressPort, &progressMessage);
  WaitPort(m_pReplyPort);
  GetMsg(m_pReplyPort);
}
