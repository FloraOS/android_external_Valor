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

#include <valor/config.h>
#include <valor/checksum.h>

#include "aassert.h"
#include "util.h"
#include "log.h"

proccess_array_t *_add_process_to_array(proccess_array_t *array, process_t process) {
    aassert(array->length >= 0);
    array->processes = (process_t *) realloc(array->processes, (array->length + 1) * sizeof(process_t));
    memcpy(&array->processes[array->length], &process, sizeof(process_t));
    ++array->length;
    return array;
}

char *_make_proc_path(char *dirname) {
    char *path = (char *) malloc((7 + strlen(dirname)) * sizeof(char));
    strcpy(path, "/proc/");
    strcat(path, dirname);
    return path;
}

bool _set_checksum(process_t *process, uint32_t chunk_size) {
    aassert(process);
    char *exe_path = (char *) malloc((5 + strlen(process->proc_path)) * sizeof(char));
    strcpy(exe_path, process->proc_path);
    strcat(exe_path, "/exe");
    FILE *exe = fopen(exe_path, "r");
    if (!exe) {
        free(exe_path);
        return false;
    }
    process->checksums = calculate_checksum_chunks(exe, chunk_size);
    fclose(exe);
    free(exe_path);
    return true;
}

proccess_array_t *_make_process_array(void) {
    proccess_array_t *array = (proccess_array_t *) malloc(sizeof(proccess_array_t));
    array->length = 0;
    array->processes = (process_t *) malloc(sizeof(process_t));
    return array;
}

char *get_process_comm(const char *proc_path) {
    char *filename = (char *) malloc(sizeof(char) * (strlen(proc_path) + 8));
    strcpy(filename, proc_path);
    strcat(filename, "/cmdline");
    FILE *file = fopen(filename, "r");
    if (!file) {
        error("Failed to open file: %s", filename);
        free(filename);
        return NULL;
    }

    char *buffer = malloc(256 * sizeof(char));
    if (!buffer) {
        cerror("Failed to allocate memory");
        fclose(file);
        free(filename);
        return NULL;
    }

    if (fgets(buffer, 256, file) == NULL) {
        debug("Failed to read from file: %s(%d: %s)", filename, errno, strerror(errno));
        fclose(file);
        free(buffer);
        free(filename);
        return NULL;
    }

    fclose(file);

    // Remove the newline character at the end of the command name, if present
    size_t len = strlen(buffer);
    if (buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    free(filename);
    return buffer;
}

proccess_array_t *get_processes(uint32_t chunk_size) {
    struct dirent *_dirent;
    DIR *dir;
    proccess_array_t *processes = _make_process_array();
    process_t current_process;

    dir = opendir("/proc");
    if (!dir) {
        cerror("opendir");
    }

    while ((_dirent = readdir(dir)) != NULL) {
        if (!is_int(_dirent->d_name)) {
            continue;
        }
        current_process.pid = atoi(_dirent->d_name);
        current_process.proc_path = _make_proc_path(_dirent->d_name);
        current_process.comm = get_process_comm(current_process.proc_path);
        if (_set_checksum(&current_process, chunk_size)) {
            processes = _add_process_to_array(processes, current_process);
        }
    }

    return processes;
}

void free_process_array(proccess_array_t *array) {
    size_t i, j;
    for (i = 0; i < array->length; ++i) {
        for (j = 0; j < array->processes[i].checksums->sz; ++j) {
            free(array->processes[i].checksums);
        }
        free(array->processes[i].comm);
        free(array->processes[i].proc_path);
    }
    free(array);
}

