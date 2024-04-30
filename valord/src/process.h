#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include <valor/checksum.h>

typedef struct {
    pid_t pid;
    char *proc_path;
    char *comm;
    array_t* checksums;
} process_t;

typedef struct {
    size_t length;
    process_t *processes;
} proccess_array_t;


proccess_array_t *get_processes(uint32_t chunk_size);
void free_process_array(proccess_array_t* array);

#endif

