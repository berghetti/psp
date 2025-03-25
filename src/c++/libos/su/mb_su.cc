#include <arpa/inet.h>
#include <psp/libos/su/MbSu.hh>

#include "fake_work.h"
#include "resp.h"
#include <leveldb/c.h>

extern leveldb_t *db;

int MbWorker::setup() {
  assert(n_peers > 0);
  // pin_thread(pthread_self(), cpu_id);

  PSP_INFO("Set up Microbenchmark worker " << worker_id);
  return 0;
}

static void do_get(char *key) {
  size_t len;
  char *value, *err = NULL;

  leveldb_readoptions_t *readoptions = leveldb_readoptions_create();

  value = leveldb_get(db, readoptions, key, strlen(key), &len, &err);
  free(value);
  free(err);

  leveldb_readoptions_destroy(readoptions);
}

static void do_scan(void) {
  const char *retr_key;
  size_t len;

  leveldb_readoptions_t *readoptions = leveldb_readoptions_create();
  leveldb_iterator_t *iter = leveldb_create_iterator(db, readoptions);

  leveldb_iter_seek_to_first(iter);
  while (leveldb_iter_valid(iter)) {
    retr_key = leveldb_iter_key(iter, &len);
    (void)retr_key;
    //#ifndef NDEBUG
    //      char *err = NULL;
    //      char *value = leveldb_get ( db, readoptions, retr_key, len, &len,
    //      &err ); assert ( !err ); printf ( "key:%s value:%s\n", retr_key,
    //      value );
    //#endif
    leveldb_iter_next(iter);
  }

  leveldb_iter_destroy(iter);
  leveldb_readoptions_destroy(readoptions);
}

static void leveldb_server(void *buff) {
#define GET 1
#define SCAN 2

  uint64_t *data = (uint64_t *)buff;
  uint32_t type = data[3];
  uint64_t key = data[5];

  switch (type) {
  case GET:
    do_get((char *)&key);
    break;
  case SCAN:
    do_scan();
    break;
  default:
    assert(0 && "Invalid request type");
  }
}

int MbWorker::process_request(unsigned long payload) {
  // AFP
  // char *id_addr = rte_pktmbuf_mtod_offset(
  //     static_cast<rte_mbuf *>((void*)payload), char *, NET_HDR_SIZE
  // );

  // char *type_addr = id_addr + sizeof(uint32_t);
  // char *req_addr = type_addr + sizeof(uint32_t) * 2; // also pass request
  // size

  uint64_t *data = rte_pktmbuf_mtod_offset(
      static_cast<rte_mbuf *>((void *)payload), uint64_t *, NET_HDR_SIZE);

  unsigned type;
  unsigned ns_sleep;
#ifndef RESP
  // type = data[3];
  ns_sleep = data[4];

  if (ns_sleep < 10000)
    type = 1;
  else
    type = 2;

    // tri-modal zippy
    // type 1 - short ; type2 - short; type 3 - long
    // if (type > 1)
    //  type--;
#else
  // resp parser
  char *resp_request = (char *)&data[6];
  struct resp_client resp;
  resp_decode(&resp, resp_request);

  char *cmd = resp.bs[0].string;
  unsigned cmd_size = resp.bs[0].size;

  ns_sleep = atounsigned(resp.bs[1].string);
  if (!strncmp(cmd, "SHORT", cmd_size)) {
    type = 1;
  } else if (!strncmp(cmd, "LONG", cmd_size)) {
    type = 2;
  } else
    type = 3;

#endif

#ifdef DB
  leveldb_server(data);
#else
  fake_work_ns(ns_sleep);
#endif

  // uint32_t spin_time = 1000;
  // unsigned int nloops = *reinterpret_cast<unsigned int *>(req_addr) * FREQ;
  // unsigned int nloops = service_time_ns * FREQ;
  PSP_DEBUG("spinning for " << nloops);
  /*
  printf("spinning for %u\n", nloops);
  double durations[1000];
  for (unsigned int i = 0 ; i < 1000; i++) {
      uint64_t start = rdtscp(NULL);
  */
  // fake_work(nloops);
  // fake_work2(*reinterpret_cast<unsigned int *>(req_addr), FREQ);
  /*
      uint64_t end = rdtscp(NULL);
      durations[i] = (end - start) / 2.5;
  }
  std::sort(durations, durations+1000);
  printf("median: %f\n", durations[500]);
  printf("p99.9: %f\n", durations[999]);
  */
  // uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
  switch (static_cast<ReqType>(type)) {
  case ReqType::SHORT:
    n_shorts++;
    break;
  case ReqType::LONG:
    n_longs++;
    break;
  case ReqType::PAGE:
    n_pages++;
    break;
  default:
    break;
  }
  n_requests++;

  // Hack response to include completion timestamp
  //*reinterpret_cast<uint32_t *> (req_addr) = 0;
  return type;
}

int MbWorker::work(int status, unsigned long payload) {
  return app_work(status, payload);
}

int MbWorker::dequeue(unsigned long *payload) { return app_dequeue(payload); }
