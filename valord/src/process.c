#include "process.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>

#include <valor/checksum.h>
#include <valor/config.h>

#include "aassert.h"
#include "util.h"
#include "log.h"


char *make_proc_path(char *dirname) {
    char *path = (char *) malloc((7 + strlen(dirname)) * sizeof(char));
    cerror("malloc");
    strcpy(path, "/proc/");
    strcat(path, dirname);
    return path;
}


/**
 * Set checksum for given process_t
 * @param process pointer to process_t struct
 * @param chunk_size size of chunk to calculate checksum
 * @return whether checksum calculation was successful
 */
bool set_checksum(process_t *process, uint32_t chunk_size) {
    aassert(process);
    char *exe_path = (char *) malloc((5 + strlen(process->proc_path)) * sizeof(char));
    cerror("malloc");
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


char *get_process_comm(const char *proc_path) {
    char *filename = (char *) malloc(sizeof(char) * (strlen(proc_path) + 8));
    strcpy(filename, proc_path);
    strcat(filename, "/cmdline");
    FILE *file = fopen(filename, "r");
    if (!file) {
#if DEBUG
        error("Failed to open file: %s", filename);
        perror("fopen");
#endif
        free(filename);
        return NULL;
    }

    char *buffer = malloc(256 * sizeof(char));
    if (!buffer) {
        cerror("malloc");
        fclose(file);
        free(filename);
        return NULL;
    }

    if (fgets(buffer, 256, file) == NULL) {
        error("Failed to read from file: %s(%d: %s)", filename, errno, strerror(errno));
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

void free_process(process_t *process) {
    free(process->comm);
    free(process->proc_path);
    if (process->checksums != NULL) {
        array_free_with_base(process->checksums);
    }
}

process_t *get_process(char *dir_name) {
    if (!is_int(dir_name)) {
        return NULL;
    }
    process_t *process = (process_t *) malloc(sizeof(process_t));
    process->proc_path = make_proc_path(dir_name);
    process->comm = get_process_comm(process->proc_path);
    process->pid = atoi(dir_name);
    return process;
}

/**
 * Gets all processes without calculating their checksum
 * @note Proccess checksums array is NULL
 * @return array_t of type process_t
 */
array_t *get_processes(void) {
    struct dirent *_dirent;
    DIR *dir;

    array_t *processes = array_create(128);
    //size_t i = 0;

    dir = opendir("/proc");
    if (!dir) {
        cerror("opendir");
    }

    for (;;) {
        _dirent = readdir(dir);
        if(!_dirent){
            break;
        }
        process_t *process = get_process(_dirent->d_name);

        if(process != NULL) {
            array_add(processes, process);
        }
    }

    closedir(dir);
    return processes;
}

void free_process_array(array_t *array) {
    size_t i = 0;
    for (; i < array->sz; ++i) {
        free_process((process_t*)array->base[i]);
    }
    array_free_with_base(array);
}

