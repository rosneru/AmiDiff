#include <clib/exec_protos.h>
#include "StopWatch.h"

// Base address of timer device; has to be global
struct Device* TimerBase = NULL;

StopWatch::StopWatch()
  : m_ClocksPerSecond(0),
    m_pMsgPort(NULL),
    m_pIORequest(NULL),
    m_bInitialized(false)
{
  // Create a message port
  m_pMsgPort = CreateMsgPort();
  if(m_pMsgPort == NULL)
  {
    return;
  }

  // Create an IORequest
  m_pIORequest = (struct IORequest*)CreateIORequest(
    m_pMsgPort, sizeof(struct timerequest));
  if(m_pIORequest == NULL)
  {
    DeleteMsgPort(m_pMsgPort);
    m_pMsgPort = NULL;

    return;
  }

  // Opening the timer.device
  BYTE res = OpenDevice(TIMERNAME, UNIT_ECLOCK, m_pIORequest , TR_GETSYSTIME);
  if(res != NULL)
  {
    DeleteIORequest(m_pIORequest);
    m_pIORequest = NULL;

    DeleteMsgPort(m_pMsgPort);
    m_pMsgPort = NULL;

    return;
  }

  // Setting the timer base
  TimerBase = m_pIORequest->io_Device;

  m_bInitialized = true;
}

StopWatch::~StopWatch()
{
  if((TimerBase != NULL) && (m_pIORequest != NULL))
  {
    CloseDevice(m_pIORequest);
    TimerBase = NULL;

    DeleteIORequest(m_pIORequest);
    m_pIORequest = NULL;
  }

  if(m_pMsgPort != NULL)
  {
    DeleteMsgPort(m_pMsgPort);
  }
}

void StopWatch::Start()
{
  if(m_bInitialized == false)
  {
    return;
  }

  // Getting and storing current eclock value
  m_ClocksPerSecond = ReadEClock(&m_StartClock);
}


double StopWatch::Pick(bool p_bKeepStartPoint)
{
  if(m_bInitialized == false)
  {
    return -1.0;
  }

  if(m_ClocksPerSecond == 0)
  {
    return -1.0;
  }

  // Reading the eclock value again
  ReadEClock(&m_StopClock);

  // Calculating elapsed time in seconds
  double seconds = m_StopClock.ev_lo - m_StartClock.ev_lo;
  seconds /= (double)m_ClocksPerSecond;

  if(p_bKeepStartPoint == false)
  {
    ReadEClock(&m_StartClock);
  }

  // Returning the elapsed time in milliseconds
  return seconds * 1000.0;
}
