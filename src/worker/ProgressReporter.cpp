
#include <stddef.h>

#include <clib/exec_protos.h>

#include "ProgressMessage.h"
#include "ProgressReporter.h"

ProgressReporter::ProgressReporter(struct MsgPort*& pProgressPort,
                                   struct MsgPort*& pReplyPort)
  : m_pProgressPort(pProgressPort),
    m_pReplyPort(pReplyPort),
    m_pProgressDescription("Progress..")
{

}

void ProgressReporter::setProgressDescription(const char *pProgressDescription)
{
  m_pProgressDescription = pProgressDescription;
}

#include <stdio.h>
void ProgressReporter::notifyProgressChanged(int progress)
{
  if ((m_pProgressPort == NULL) || (m_pReplyPort == NULL))
  {
    printf("m_pProgressPort=%zu, m_pReplyPort=%zu\n", m_pProgressPort, m_pReplyPort);
    return;
  }

  // Creating and initializing the progress message
  struct ProgressMessage progressMessage;
  progressMessage.mn_ReplyPort = m_pReplyPort;
  progressMessage.progress = progress;
  progressMessage.pDescription = m_pProgressDescription;

  // Sending the progress message, waiting for the answer and taking the
  // answer off the queue
  PutMsg(m_pProgressPort, &progressMessage);
  WaitPort(m_pReplyPort);
  GetMsg(m_pReplyPort);
}
