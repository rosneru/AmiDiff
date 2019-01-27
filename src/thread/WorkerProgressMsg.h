#ifndef WORKER_PROGRESS_MSG
#define WORKER_PROGRESS_MSG

#include <exec/ports.h>

struct WorkerProgressMsg : public Message
{
  long progress;
  const char *pDescription;
  const char *pResult;
};


#endif
