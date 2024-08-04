#include <psp/libos/su/AFP_RocksdbSu.hh>
#include <arpa/inet.h>

int
AFP_RdbWorker::setup ()
{
  assert (n_peers > 0);
  // pin_thread(pthread_self(), cpu_id);


  PSP_INFO ("Set up AFP RocksDB worker " << worker_id);
  return 0;
}

void
AFP_RdbWorker::do_get (rocksdb_readoptions_t *ro, char *key)
{
  size_t len;
  char *value, *err = NULL;

  value = rocksdb_get (db, ro, key, strlen (key), &len, &err);
  free (value);
  free (err);
}

void
AFP_RdbWorker::do_scan ( rocksdb_readoptions_t *ro )
{
  const char *retr_key;
  size_t len;

  rocksdb_iterator_t *iter = rocksdb_create_iterator (db, ro);

  for (rocksdb_iter_seek_to_first (iter); rocksdb_iter_valid (iter);
       rocksdb_iter_next (iter))
    {
      retr_key = rocksdb_iter_key (iter, &len);
      (void)retr_key;
    }

  rocksdb_iter_destroy (iter);
}

int
AFP_RdbWorker::process_request (unsigned long payload)
{
  uint64_t *data = rte_pktmbuf_mtod_offset (
      static_cast<rte_mbuf *> ((void *)payload), uint64_t *, NET_HDR_SIZE);

  uint32_t type = data[3];
  uint64_t key = data[4];

#define GET 1
#define SCAN 2

  rocksdb_readoptions_t *readoptions = rocksdb_readoptions_create();
  switch (type)
    {
    case GET:
      do_get (readoptions, (char *)&key);
      n_gets++;
      break;
    case SCAN:
      do_scan (readoptions);
      n_scans++;
      break;
    default:
      break;
    }
  n_requests++;
  rocksdb_readoptions_destroy(readoptions);

  return 0;
}

int
AFP_RdbWorker::work (int status, unsigned long payload)
{
  return app_work (status, payload);
}

int
AFP_RdbWorker::dequeue (unsigned long *payload)
{
  return app_dequeue (payload);
}
