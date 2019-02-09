#ifndef PROGRESS_REPORTER
#define PROGRESS_REPORTER


/**
 * This is an abstract class / interface to allow an object to report
 * its progress. It should be used together with BackgroundWorker when
 * performing time-consuming operations.
 *
 * @author Uwe Rosner
 * @date 26/01/2019
 */
class ProgressReporter
{
public:
  virtual void notifyProgressChanged(int p_Progress) = 0;
};

#endif
