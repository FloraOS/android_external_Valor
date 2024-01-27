#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include "checksum.h"

typedef struct {
    pid_t pid;
    char *proc_path;
    checksum_t checksum;
} process_t;

typedef struct {
    size_t length;
    process_t *processes;
} proccess_array_t;


proccess_array_t *get_processes(void);
void free_process_array(proccess_array_t* array);

#endif

