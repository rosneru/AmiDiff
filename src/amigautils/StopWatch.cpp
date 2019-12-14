#include <clib/exec_protos.h>
#include "StopWatch.h"

// Base address of timer device; has to be global
struct Library* TimerBase = NULL;

StopWatch::StopWatch()
  : m_ClocksPerSecond(0),
    m_pMsgPort(NULL),
    m_pTimeRequest(NULL),
    m_bInitialized(false)
{

}

StopWatch::~StopWatch()
{
  freeTimerDevice();
}


void StopWatch::Start()
{
  if(!m_bInitialized)
  {
    initTimerDevice();
    if(!m_bInitialized)
    {
      return;
    }
  }

  // Getting and storing current eclock value
  m_ClocksPerSecond = ReadEClock(&m_StartClock);
}


long StopWatch::Pick(bool bKeepStartPoint)
{
  if(!m_bInitialized)
  {
    return -1;
  }

  if(m_ClocksPerSecond == 0)
  {
    return -1;
  }

  // Reading the eclock value again
  ReadEClock(&m_StopClock);

  // Calculating elapsed time in milliseconds
  long long millisecs = m_StopClock.ev_lo - m_StartClock.ev_lo;

  millisecs *= 1000;

  millisecs /= m_ClocksPerSecond;

  if(bKeepStartPoint == false)
  {
    ReadEClock(&m_StartClock);
  }

  return static_cast<LONG>(millisecs);
}


void StopWatch::initTimerDevice()
{
  if(m_bInitialized)
  {
    return;
  }

  // Create a message port
  m_pMsgPort = CreateMsgPort();
  if(m_pMsgPort == NULL)
  {
    return;
  }

  // Create an IORequest
  m_pTimeRequest = (struct timerequest*)
    CreateIORequest(m_pMsgPort, sizeof(struct timerequest));

  if(m_pTimeRequest == NULL)
  {
    freeTimerDevice();
    return;
  }

  // Opening the timer.device
  BYTE res = OpenDevice(TIMERNAME,
                        UNIT_ECLOCK,
                        (struct IORequest*) m_pTimeRequest,
                        TR_GETSYSTIME);
  if(res != NULL)
  {
    freeTimerDevice();
    return;
  }

  m_pTimeRequest->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

  // Setting the timer base
  TimerBase = (struct Library*)m_pTimeRequest->tr_node.io_Device;

  m_bInitialized = true;
}

void StopWatch::freeTimerDevice()
{
  if(TimerBase != NULL)
  {
    CloseDevice((struct IORequest*)m_pTimeRequest);
    TimerBase = NULL;
  }

  if(m_pTimeRequest != NULL)
  {
    DeleteIORequest(m_pTimeRequest);
    m_pTimeRequest = NULL;
  }

  if(m_pMsgPort != NULL)
  {
    DeleteMsgPort(m_pMsgPort);
    m_pMsgPort = 0;
  }

  m_bInitialized = false;
}

