#include "process.h"
#include "config.h"
#include "aassert.h"

#include "db/db.h"

#include <android/log.h>

int main(void){
  __android_log_print(ANDROID_LOG_INFO, MODNAME, "starting up...");
  database* db = db_init(DB_SIZE);
  FILE* file = fopen(DB_FILE, "r");
  cerror("fopen");
  db_read(db, file);
  __android_log_print(ANDROID_LOG_INFO, MODNAME, "loaded database from %s", DB_FILE);
  
  size_t i;
  checksum_t chksum;

  for(;;){
    proccess_array_t* processes = get_processes();
    for(i = 0; i < processes->length; ++i){
      printf("%d\n", i);
      __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "searching for checksum %ld, pid=%d", processes->processes[i].checksum, processes->processes[i].pid);
      db_entry_t* entry = db_get_entry(db, processes->processes[i].checksum);
      if(entry){
        __android_log_print(ANDROID_LOG_WARN, MODNAME, "detected threat %s, killing immediatly", db->names[entry->id]);
        kill(processes->processes[i].pid, SIGKILL);
      }
    }
    printf("%d\n", processes->length);
    sleep(IDLE_TIME);
  }
}
