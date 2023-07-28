#include <psp/libos/su/ApacheSu.hh>
#include <arpa/inet.h>
#include "fake_work.h"

int ApacheWorker::setup() {
    assert(n_peers > 0);
    //pin_thread(pthread_self(), cpu_id);

    PSP_INFO("Set up Microbenchmark worker " << worker_id);
    return 0;
}

int ApacheWorker::process_request(unsigned long payload) {
    char *id_addr = rte_pktmbuf_mtod_offset(
        static_cast<rte_mbuf *>((void*)payload), char *, NET_HDR_SIZE
    );

    char *type_addr = id_addr + sizeof(uint32_t);
    char *req_addr = type_addr + sizeof(uint32_t) * 2; // also pass request size

    unsigned int nloops = 1000;

    fake_work(nloops);

    uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
    switch(static_cast<ReqType>(type)) {
        case ReqType::Apache_REQUEST:
            n_shorts++;
            break;
        default:
            break;
    }
    n_requests++;

    // Hack response to include completion timestamp
    *reinterpret_cast<uint32_t *> (req_addr) = 0;
    return 0;
}

int ApacheWorker::work(int status, unsigned long payload) {
    return app_work(status, payload);
}

int ApacheWorker::dequeue(unsigned long *payload) {
    return app_dequeue(payload);
}
