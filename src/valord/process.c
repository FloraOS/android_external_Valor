#include "process.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <android/log.h>

#include "aassert.h"
#include "checksum.h"
#include "util.h"
#include "config.h"

proccess_array_t* _add_process_to_array(proccess_array_t* array, process_t process){
  aassert(array->length >= 0);
  array->processes = (process_t*)realloc(array->processes, (array->length+1)*sizeof(process_t));
  memcpy(&array->processes[array->length], &process, sizeof(process_t));
  ++array->length;
  return array;
}

char* _make_proc_path(char* dirname){
  char* path = (char*)malloc((7 + strlen(dirname))*sizeof(char));
  strcpy(path, "/proc/");
  strcat(path, dirname);
  return path;
}

void _set_checksum(process_t* process){
  aassert(process);
  char* exe_path = (char*)malloc((5 + strlen(process->proc_path))*sizeof(char));
  strcpy(exe_path, process->proc_path);
  strcat(exe_path, "/exe");
  FILE* exe = fopen(exe_path, "r");
  if(!exe){
#if DEBUG
    __android_log_print(ANDROID_LOG_DEBUG, MODNAME, "fopen: %s: %s(%d)  [%s:%d]", exe_path, strerror(errno), errno, __FILE__, __LINE__);	  
#endif
    free(exe_path);    
    return ;
  }
  checksum_file(exe, &process->checksum);
  fclose(exe);
  free(exe_path);
}

proccess_array_t* _make_process_array(void){
  proccess_array_t* array = (proccess_array_t*)malloc(sizeof(proccess_array_t));
  array->length = 0;
  array->processes = (process_t*)malloc(sizeof(process_t));
  return array;
}

proccess_array_t* get_processes(void){
  struct dirent* _dirent;
  DIR* dir;
  proccess_array_t* processes = _make_process_array();
  process_t current_process;
  
  dir = opendir("/proc");
  if(!dir){
    cerror("opendir");
  }

  while((_dirent = readdir(dir)) != NULL){
    if(!is_int(_dirent->d_name)){
      continue;
    }
    current_process.pid = atoi(_dirent->d_name);
    current_process.proc_path = _make_proc_path(_dirent->d_name);
    _set_checksum(&current_process);
    processes = _add_process_to_array(processes, current_process);
  }
  
  return processes;
}

void free_process_array(proccess_array_t* array){
  size_t i;
  for(i = 0; i < array->length; ++i){
    free(array->processes[i].proc_path);
  }
  free(array);
}


