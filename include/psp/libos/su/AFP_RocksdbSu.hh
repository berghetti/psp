#ifndef AFP_RDB_SU_H_
#define AFP_RDB_SU_H_

#include <psp/annot.h>
#include <psp/libos/Request.hh>
#include <psp/libos/persephone.hh>

#include <c.h> // RocksDB C bindings

#define MAX_RESPONSE_SIZE 32
#define RESPONSE_TPL "Spinned for %u"

class AFP_RdbWorker : public Worker {
    public: AFP_RdbWorker() : Worker(WorkerType::RDB) {}
    public : ~AFP_RdbWorker() {
                log_info(
                    "RocksDB worker %d processed %u requests (%u GETs, %u SCANs), dropped %lu requests",
                    worker_id, n_requests, n_gets, n_scans, n_drops
                );
             }
    private: uint32_t n_requests = 0;
    private: uint32_t n_gets = 0;
    private: uint32_t n_scans = 0;
    private: int setup() override;
    private: int dequeue(unsigned long *payload);
    private: int work(int status, unsigned long payload) override;
    private: int process_request(unsigned long payload) override;
    private: void do_get(rocksdb_readoptions_t *ro, char *key);
    private: void do_scan(rocksdb_readoptions_t *ro);
    public: rocksdb_t *db;
};

#endif // RDB_SU_H_
