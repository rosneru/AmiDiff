#ifndef PROGRESS_REPORTER
#define PROGRESS_REPORTER


/**
 * This is an abstract class / interface to allow an object to report
 * its progress. It should be used together with a background worker 
 * when performing time-consuming operations.
 *
 * @author Uwe Rosner
 * @date 26/01/2019
 */
class ProgressReporter
{
public:
  ProgressReporter(struct MsgPort*& pProgressPort, 
                   struct MsgPort*& pReplyPort);

  virtual void notifyProgressChanged(int progress);
  void setProgressDescription(const char* pProgressDescription);

private:
  struct MsgPort*& m_pProgressPort;
  struct MsgPort*& m_pReplyPort;
  const char* m_pProgressDescription;

};

#endif
