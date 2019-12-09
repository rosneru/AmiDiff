#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <dos/dosextens.h>
#include <exec/ports.h>

#include "ProgressReporter.h"
#include "SimpleString.h"
#include "WorkerStartupMsg.h"

/**
 * This is an abstract class to provide background worker functionality
 * e.g. for time-consuming operations.
 *
 * To implement this functionality a class can inherit from this class
 * fulfilling the following conditions:
 *
 * 1. In its constructor it the has to pass a message port to the base
 * constructor to which the progress messages will be sent. These
 * messages have to be received (using Wait()) and replied (using
 * ReplyMsg) in the application event loop. The progress messages are
 * of type WorkerProgresMsg, @see WorkerProgresMsg.h
 *
 * 2. It must implement the doWork() method in which it should perform
 * the time-consuming operations.
 *
 * 3. It can send progress messages using the method sendProgress(int)
 * where progress should be a value between 0..100
 *
 * The WorkerBase itself inherits from the abstract class 
 * ProgressReporter and implements its notifyProgressChanged() method.
 * So a class with a time consuming operation can hold a pointer to
 * the WorkerBase (e.g. define a SetProgressReporter() method) and
 * then use the notifyProgressChanged() method of the ProgressReporter
 * to trigger the WorkerBase to send the WorkerProgresMsg's to
 * the progress message port.
 *
 * @author Uwe Rosner
 * @date 24/01/2019
 */
class WorkerBase : public ProgressReporter
{
public:

  WorkerBase(struct MsgPort*& pProgressPort);
  virtual ~WorkerBase();

  /**
   * Creates and starts the background process.
   */
  bool Run();

protected:
  WorkerStartupMsg* m_pStartupMsg;
  struct MsgPort*& m_pProgressPort;
  struct MsgPort* m_pReplyPort;

  SimpleString m_LastError;
  void setProgressDescription(const char* pProgressDescription);

  virtual void doWork() = 0;

  /**
   * Sending a message about progres to the port given in the
   * constructor.
   */
  virtual void notifyProgressChanged(int progress);


private:
  struct Process* m_pBackgrProcess;
  const char* m_pProgressDescription;
  bool m_bExitRequested;

  static void startup();
};

#endif
