//
// Created by yigonghu on 11/11/22.
//

#include "psp/libos/su/PostgreSQLSu.hh"
#include "libpq-fe.h"

int PostgreSQLWorker::setup() {
  assert(n_peers > 0);
  //pin_thread(pthread_self(), cpu_id);

  PSP_INFO("Set up PostgreSQL worker " << worker_id);
  return 0;
}

static void
exit_nicely(PGconn *conn)
{
  PQfinish(conn);
  exit(1);
}


int PostgreSQLWorker::process_request(unsigned long payload) {
  char *id_addr = rte_pktmbuf_mtod_offset(
      static_cast<rte_mbuf *>((void*)payload), char *, NET_HDR_SIZE
  );
  const char *conninfo;
  PGconn     *conn;
  PGresult   *res;
  char *type_addr = id_addr + sizeof(uint32_t);
  char *req_addr = type_addr + sizeof(uint32_t) * 2; // also pass request size

  conninfo = "dbname = postgres";
  conn = PQconnectdb(conninfo);
  if (PQstatus(conn) != CONNECTION_OK) {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  uint32_t type = *reinterpret_cast<uint32_t *>(type_addr);
  uint32_t query_type = *reinterpret_cast<unsigned int *>(req_addr);
  char* query;
  switch(static_cast<ReqType>(type)) {
  case ReqType::PostgreSQL_UPDATE: {
    if (query_type == 0) {
      int id = rand() % 1000000 + 1;
      std::string q = "UPDATE sbtest1 SET k=k+1 WHERE id=" + std::to_string(id) ;
      strcpy(query, q.c_str());
    } else if (query_type == 1) {
      int id = rand() % 1000 + 1;
      std::string q = "UPDATE sbtest1 SET k=k+1 WHERE id=" + std::to_string(id) ;
      strcpy(query, q.c_str());
    }
    res = PQexec(conn, query);
    break;
  }
    case ReqType::PostgreSQL_READ: {
      if (query_type == 0) {
        strcpy(query,"EXPLAIN ANALYZE SELECT * FROM plan WHERE typ = 3 AND dat IS NOT NULL");
      } else if (query_type == 1) {
        PQexec(conn, "BEGIN");
        PQexec(conn, "select 1 from sbtest1 for update;");
        PQexec(conn, "SELECT pg_sleep(3)");
        PQexec(conn, "commit");
      } else if (query_type == 2) {
        PQexec(conn, "BEGIN");
        PQexec(conn, "select 1 from sbtest1 for share;");
        PQexec(conn, "SELECT pg_sleep(3)");
        PQexec(conn, "commit");
      } else if (query_type == 3) {
        PQexec(conn, "VACUUM FULL sbtest1");
      }
      res = PQexec(conn, query);
      break;
    }
    case ReqType::PostgreSQL_INSERT: {
      if (query_type == 0) {
        PQexec(conn, "BEGIN");
        PQexec(conn, "INSERT INTO plan SELECT id + 200001,typ,current_date + id * '1 seconds'::interval ,val FROM plan;");
        PQexec(conn, "SELECT pg_sleep(10)");
        PQexec(conn, "delete from plan");
        PQexec(conn, "COPY plan FROM './data0/dump/plan1.dat' (DELIMITER ',', NULL '')");
        PQexec(conn, "commit");
      }  else if (query_type == 1) {
        PSP_INFO("query is " << query_count);
        query_count++;
//        PQexec(conn, "BEGIN");
//        PQexec(conn, "INSERT INTO plan SELECT id + 2000001,typ,current_date + id * '1 seconds'::interval ,val FROM plan where id < 500000");
//        PQexec(conn, "commit");
      }
     break;
    }
  }

  PQclear(res);
  PQfinish(conn);

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
