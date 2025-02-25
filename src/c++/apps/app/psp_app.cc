#include "psp_app.hh"

#include <csignal>

#include <leveldb/c.h>

leveldb_t *db;

int main(int argc, char *argv[]) {
  if (TRACE)
    PSP_INFO("Starting PSP application with TRACE on");
#ifdef LOG_DEBUG
  log_info("Starting PSP application with LOG_DEBUG on");
#endif

#ifdef DB
  // Initialize levelDB
  leveldb_options_t *options = leveldb_options_create();

  // open DB
   char *err = NULL;
   const char *DBPath = "/tmpfs/my_db";
   db = leveldb_open ( options, DBPath, &err );
   if ( err ){
    fprintf ( stderr,
              "Error to open database:\n%s\n",
              err );
    exit ( 1 );
  }
#endif

  PspApp app(argc, argv);

  if (std::signal(SIGINT, Psp::stop_all) == SIG_ERR)
    log_error("can't catch SIGINT");
  if (std::signal(SIGTERM, Psp::stop_all) == SIG_ERR)
    log_error("can't catch SIGTERM");

  /* Start all workers */
  for (unsigned int i = 0; i < total_workers; ++i) {
    if (workers[i]->launch() != 0) {
      app.psp->stop_all(SIGTERM);
      break;
    }
  }

  /* Join threads */
  for (unsigned int i = 0; i < total_workers; ++i) {
    workers[i]->join();
    delete workers[i];
  }

#ifdef DB
   puts ("Closking DB");
   leveldb_close (db);
#endif

  return 0;
}
