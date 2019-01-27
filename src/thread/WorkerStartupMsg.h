#ifndef WORKER_STARTUP_MSG
#define WORKER_STARTUP_MSG

#include <exec/ports.h>

class BackgroundWorker;

struct WorkerStartupMsg : public Message 
{
  class BackgroundWorker* that;
};

#endif
