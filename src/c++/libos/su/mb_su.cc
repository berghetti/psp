#include "fake_work.h"
#include <arpa/inet.h>
#include <psp/libos/su/MbSu.hh>

int
MbWorker::setup ()
{
  assert (n_peers > 0);
  // pin_thread(pthread_self(), cpu_id);

  PSP_INFO ("Set up Microbenchmark worker " << worker_id);
  exit (1); // not use this code
  return 0;
}

int
MbWorker::process_request (unsigned long payload)
{
  return 0;
}

int
MbWorker::work (int status, unsigned long payload)
{
  return app_work (status, payload);
}

int
MbWorker::dequeue (unsigned long *payload)
{
  return app_dequeue (payload);
}
