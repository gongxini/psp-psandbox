//
// Created by yigonghu on 11/11/22.
//

#include <psp/libos/persephone.hh>
#include <psp/libos/Request.hh>
#include <psp/annot.h>

#ifndef PERSEPHONE_SRC_C_LIBOS_SU_PostgreSQL_SU_H_
#define PERSEPHONE_SRC_C_LIBOS_SU_PostgreSQL_SU_H_

class PostgreSQLWorker : public Worker {
 public: PostgreSQLWorker() : Worker(WorkerType::MySQL) {}

 public : ~PostgreSQLWorker() {
    log_info(
        "PostgreSQLWorker worker %d processed %u requests (%u MySQL_UPDATE, %u MySQL_READ_UPDATE), dropped %lu requests for 60 seconds",
        worker_id, n_requests, n_noisy, n_victim, n_drops
    );
  }

 private: uint32_t n_requests = 0;
 private: uint32_t n_noisy = 0;
 private: uint32_t n_victim = 0;
 private: int setup() override;
 private: int dequeue(unsigned long *payload);
 private: int work(int status, unsigned long payload) override;
 private: int process_request(unsigned long payload) override;
};

#endif //PERSEPHONE_SRC_C_LIBOS_SU_MYSQL_SU_H_
