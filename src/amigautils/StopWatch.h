#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include <clib/timer_protos.h>
#include <clib/exec_protos.h>
#include <exec/ports.h>
#include <exec/io.h>
/**
 * Class for an easy to use stop watch using the timer.device
 *
 * @author Uwe Rosner
 * @date 21/04/2018
 */
class StopWatch
{
public:
  /**
   * Creates the stopwatch, initializes the needed MessagePort and
   * opens the timer.device
   */
  StopWatch();

  /**
   * Destroys the the stopwatch and disposed all opened resources
   */
  ~StopWatch();

  /**
   * Starts or re-starts the stop watch.
   */
  void Start();

  /**
   * Stops the stop watch and returns the elapsed time
   *
   * @returns
   * Elapsed time in milliseconds or -1.0 if an error occurred
   */
  double Stop();

private:
  ULONG m_ClocksPerSecond;
  struct MsgPort* m_pMsgPort;
  struct IORequest* m_pIORequest;
  bool m_bInitialized;
  struct EClockVal m_StartClock;
  struct EClockVal m_StopClock;
};

#endif

