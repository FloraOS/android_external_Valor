#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include <valor/array.h>

typedef struct {
    pid_t pid;
    char *proc_path;
    char *comm;
} process_t;


array_t *get_processes(void);
void free_process_array(array_t* array);

#endif

