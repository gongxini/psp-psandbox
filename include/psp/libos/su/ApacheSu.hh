#ifndef MB_SU_H_
#define MB_SU_H_

#include <psp/libos/persephone.hh>
#include <psp/libos/Request.hh>
#include <psp/annot.h>

#define MAX_RESPONSE_SIZE 32
#define RESPONSE_TPL "Spinned for %u"

class ApacheWorker : public Worker {
    public: ApacheWorker() : Worker(WorkerType::MBK) {}
    public : ~ApacheWorker() {
                log_info(
                    "MB worker %d processed %u requests (%u shorts, %u longs), dropped %lu requests",
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

#endif //MB_SU_H_
