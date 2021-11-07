#include "process.h"
#include "config.h"
#include "aassert.h"

#include "db/db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <android/log.h>

int main(void){
  __android_log_print(ANDROID_LOG_INFO, MODNAME, "starting up...");
  database* db = db_init(DB_SIZE);
  FILE* file = fopen(DB_FILE, "r");
  if(!file){
     cerror("fopen");
     __android_log_print(ANDROID_LOG_FATAL, MODNAME, "Failed to open %s for reading!", DB_FILE);
     return -1;
  }
  db_read(db, file);
  fclose(file);
  __android_log_print(ANDROID_LOG_INFO, MODNAME, "loaded database from %s", DB_FILE);
  
  size_t i;

  for(;;){
    proccess_array_t* processes = get_processes();
//    __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "Found %d processes", processes->length);
//    printf("\nAbout to eneter loop\n");
    for(i = 0; i < processes->length; ++i){
//      printf("Loop: %d\n", i);
      db_entry_t* entry = db_get_entry(db, processes->processes[i].checksum);
//      printf("Got entry!");
//      __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "PID=%d, chksum=%d",processes->processes[i].pid, processes->processes[i].checksum);
      if(entry){
        __android_log_print(ANDROID_LOG_WARN, MODNAME, "detected threat %s, killing immediatly", db->names[entry->id]);
        if(kill(processes->processes[i].pid, SIGKILL)){
		      __android_log_print(ANDROID_LOG_ERROR, MODNAME, "Failed to kill %d: kill: %s(%d)", processes->processes[i].pid, strerror(errno), errno);
	      }
      }
    }
    sleep(IDLE_TIME);
  }
}
