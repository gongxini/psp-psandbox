//
// Created by yigonghu on 11/7/22.
//

#include "psp/libos/su/MySQLSu.hh"
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


int MySQLWorker::setup() {
  assert(n_peers > 0);
  //pin_thread(pthread_self(), cpu_id);

  PSP_INFO("Set up MySQL worker " << worker_id);
  return 0;
}

int MySQLWorker::process_request(unsigned long payload) {
  char *id_addr = rte_pktmbuf_mtod_offset(
      static_cast<rte_mbuf *>((void*)payload), char *, NET_HDR_SIZE
  );

  char *type_addr = id_addr + sizeof(uint32_t);
  char *req_addr = type_addr + sizeof(uint32_t) * 2; // also pass request size

  try {
      sql::Driver *driver;
      sql::Connection *con;
      sql::Statement *stmt;
      sql::ResultSet *res;

      /* Create a connection */
      driver = get_driver_instance();
      con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
      /* Connect to the MySQL test database */
      con->setSchema("test");

      stmt = con->createStatement();
    switch(static_cast<ReqType>(type)) {
      case ReqType::NOISY:
        res = stmt->executeQuery("select count(*) from sbtest1 for update;");
        break;
      case ReqType::VICTIM:
        res = stmt->executeQuery("UPDATE sbtest1 SET k=k+1 WHERE id=10000;");
        break;
      default:
        break;
    }

      while (res->next()) {
        PSP_INFO("MySQL replies: " <<  res->getString("_message"));
      }
      delete res;
      delete stmt;
      delete con;

  } catch (sql::SQLException &e) {
    PSP_INFO("connection error");
  }


  uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
  switch(static_cast<ReqType>(type)) {
    case ReqType::NOISY:
      n_noisy++;
      break;
    case ReqType::VICTIM:
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

int MySQLWorker::work(int status, unsigned long payload) {
  return app_work(status, payload);
}

int MySQLWorker::dequeue(unsigned long *payload) {
  return app_dequeue(payload);
}
