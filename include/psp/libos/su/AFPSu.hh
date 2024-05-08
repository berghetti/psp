#ifndef AFP_SU_H_
#define AFP_SU_H_

#include <psp/libos/persephone.hh>
#include <psp/libos/Request.hh>
#include <psp/annot.h>

#define MAX_RESPONSE_SIZE 32
#define RESPONSE_TPL "Spinned for %u"

class AFPWorker : public Worker {
    public: AFPWorker() : Worker(WorkerType::MBK) {}
    public : ~AFPWorker() {
                log_info(
                    "AFP worker %d processed %u requests (%u shorts, %u longs), dropped %lu requests",
                    worker_id, n_requests, n_shorts, n_longs, n_drops
                );
             }
    private: uint32_t n_requests = 0;
    private: uint32_t n_shorts = 0;
    private: uint32_t n_longs = 0;
    private: int setup() override;
    private: int dequeue(unsigned long *payload);
    private: int work(int status, unsigned long payload) override;
    private: int process_request(unsigned long payload) override;
};

#endif //AFP_SU_H_
