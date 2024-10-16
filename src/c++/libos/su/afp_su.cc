#include "fake_work.h"
#include <arpa/inet.h>
#include <psp/libos/su/AFPSu.hh>

int
AFPWorker::setup ()
{
  assert (n_peers > 0);
  // pin_thread(pthread_self(), cpu_id);

  PSP_INFO ("Set up AFP Microbenchmark worker " << worker_id);
  return 0;
}

int
AFPWorker::process_request (unsigned long payload)
{
  uint64_t *data = rte_pktmbuf_mtod_offset (
      static_cast<rte_mbuf *> ((void *)payload), uint64_t *, NET_HDR_SIZE);

  unsigned request_type = data[3];
  unsigned service_time_ns = data[5];

  // printf("type: %u sv: %u\n", request_type, service_time_ns);

  fake_work_ns (service_time_ns);

  switch (static_cast<ReqType> (request_type))
    {
    case ReqType::SHORT:
      n_shorts++;
      break;
    case ReqType::LONG:
      n_longs++;
      break;
    default:
      break;
    }
  n_requests++;

  return 0;
}

int
AFPWorker::work (int status, unsigned long payload)
{
  return app_work (status, payload);
}

int
AFPWorker::dequeue (unsigned long *payload)
{
  return app_dequeue (payload);
}
