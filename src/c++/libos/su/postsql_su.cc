//
// Created by yigonghu on 11/11/22.
//

#include "psp/libos/su/PostgreSQLSu.hh"
#include <pqxx/pqxx>




int PostgreSQLWorker::setup() {
  assert(n_peers > 0);
  //pin_thread(pthread_self(), cpu_id);

  PSP_INFO("Set up PostgreSQL worker " << worker_id);
  return 0;
}

int PostgreSQLWorker::process_request(unsigned long payload) {
  char *id_addr = rte_pktmbuf_mtod_offset(
      static_cast<rte_mbuf *>((void*)payload), char *, NET_HDR_SIZE
  );

  char *type_addr = id_addr + sizeof(uint32_t);
  char *req_addr = type_addr + sizeof(uint32_t) * 2; // also pass request size


  pqxx::connection C("dbname = testdb user = postgres password = cohondob \
      hostaddr = 127.0.0.1 port = 5432");
  if (C.is_open()) {
    pqxx::work pqxx::W(C);
    uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
    char* sql;
    switch(static_cast<ReqType>(type)) {
    case ReqType::PostgreSQL_READ_UPDATE:
      sql = "select count(*) from sbtest1 for update;";
      break;
    case ReqType::PostgreSQL_UPDATE:
      sql = "UPDATE sbtest1 SET k=k+1 WHERE id=10000;";
      break;
    case ReqType::PostgreSQL_READ_LOCK:
      sql = "select count(*) from sbtest1 LOCK IN SHARE MODE;";
      break;
    case ReqType::PostgreSQL_TRANSACTION:
      sql = "INSERT INTO plan SELECT id + 200001,typ,current_date + id * '1 seconds'::interval ,val FROM plan;\
                               SELECT pg_sleep(10);\
                               delete from plan;\
                               COPY plan FROM './data0/dump/plan1.dat' (DELIMITER ',', NULL '');";
      break;
    case ReqType::PostgreSQL_VACUUM:
      sql = "VACUUM FULL sbtest1;";
      break;
    default:
      break;
    }
    W.exec( sql );
    W.commit();
  } else {
    std::cout << "Can't open database\n";
    return 1;
  }
  C.disconnect ();

//  while (res->next()) {
//    PSP_INFO("PostgreSQL replies: " <<  res->getString("_message"));
//  }


  uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
  switch(static_cast<ReqType>(type)) {
    case ReqType::PostgreSQL_READ_UPDATE:
      n_noisy++;
      break;
    case ReqType::PostgreSQL_UPDATE:
      n_victim++;
      break;
    default:
      break;
  }
  n_requests++;

  // Set response size to 0
  *reinterpret_cast<uint32_t *> (req_addr) = 0;
  return 0;
}

int PostgreSQLWorker::work(int status, unsigned long payload) {
  return app_work(status, payload);
}

int PostgreSQLWorker::dequeue(unsigned long *payload) {
  return app_dequeue(payload);
}
