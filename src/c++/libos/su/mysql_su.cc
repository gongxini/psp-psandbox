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
  sql::Driver *driver;
  sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;
  try {


  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306", "yigonghu", "");
  /* Connect to the MySQL test database */
  con->setSchema("test");

  stmt = con->createStatement();
  uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
  uint32_t query_type = *reinterpret_cast<unsigned int *>(req_addr);
  //PSP_INFO("type " << type << " query " << query_type)
  switch(static_cast<ReqType>(type)) {
  case ReqType::MySQL_UPDATE: {
    std::string query;
    if (query_type == 0) {
      int id = rand() % 10000 + 1;
      query = "UPDATE sbtest1 SET k=k+1 WHERE id=" + std::to_string(id) + ";";
    } else if (query_type == 1) {
      query = "UPDATE sbtest1 SET k=k+1 WHERE id=1;";
    }
    stmt->executeUpdate(query);
    break;
  }
    case ReqType::MySQL_READ: {
      std::string query;
      if (query_type == 0) {
        query = "select count(*) from sbtest1 for update;";
        stmt->executeQuery(query);
      } else if(query_type == 1) {
        query = "select count(*) from sbtest1 LOCK IN SHARE MODE;";
        stmt->executeQuery(query);
      } else if (query_type == 2) {
        query = "START TRANSACTION;";
        stmt->executeUpdate(query);
        query = "select c from sbtest1 limit 1;";
        stmt->executeQuery(query);
        query = "select sleep(10);";
        stmt->executeQuery(query);
        query = "commit;";
        stmt->executeUpdate(query);
      } else if (query_type == 3) {
        int id = rand() % 10000 + 1;
        query = "select * from sbtest1 where id = " + std::to_string(id) + ";";
        stmt->executeQuery(query);
      } else if (query_type == 4) {
        int table_id = rand() % 5 + 2;
        query = "select * from sbtest" + std::to_string(table_id) + " where id < 100 for update" ;
        stmt->executeQuery(query);
      }
      break;
    }
    case ReqType::MySQL_INSERT: {
      int k =  rand() % 10000 + 1;
      int c =  rand() % 1000000 + 1;
      int pad =  rand() % 1000000 + 1;
      int table_id = rand() % 64 + 1;
      std::string query = "INSERT INTO sbtest" + std::to_string(table_id) + " (id, k, c, pad) VALUES ( 0," + std::to_string(k) + ",\'" + std::to_string(c) + "\', \'" + std::to_string(pad) + "\')";
      stmt->executeUpdate(query);
      break;
    }
    default:
      break;
  }

//  while (res->next()) {
//    PSP_INFO("MySQL replies: " <<  res->getString("_message"));
//PSP_INFO("MySQL Finish: " <<  res->getString("_message"));
//  }


  delete stmt;
  delete con;

  } catch (sql::SQLException &e) {
    PSP_INFO("# ERR: SQLException in " << __FILE__ <<  "(" << __FUNCTION__ << ") on line " << __LINE__ )
    PSP_INFO("# ERR: " << e.what() <<  "(MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << ")" << worker_id)
    sleep(100);
    delete stmt;
    delete con;
  }

//  switch(static_cast<ReqType>(type)) {
//    case ReqType::MySQL_READ_UPDATE:
//      n_noisy++;
//      break;
//    case ReqType::MySQL_UPDATE:
//      n_victim++;
//      break;
//    default:
//      break;
//  }
//  n_requests++;

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
