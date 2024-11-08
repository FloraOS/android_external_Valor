#include "process.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>

#include <valor/config.h>

#include "aassert.h"
#include "util.h"
#include "log.h"


char *make_proc_path(char *dirname) {
    reset_errors;
    char *path = (char *) malloc((7 + strlen(dirname)) * sizeof(char));
    cerror("malloc");
    strcpy(path, "/proc/");
    strcat(path, dirname);
    return path;
}

char *get_process_comm(const char *proc_path) {
    char *filename = (char *) malloc(sizeof(char) * (strlen(proc_path) + 9));
    strcpy(filename, proc_path);
    strcat(filename, "/cmdline");
    FILE *file = fopen(filename, "r");
    if (!file) {
#if DEBUG
        error("Failed to open file: %s", filename);
        valor_perror("fopen");
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
#if DEBUG
        error("Failed to read from file: %s(%d: %s)", filename, errno, strerror(errno));
        error("ferror: %d", ferror(file));
#endif
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

/**
 * Frees process_t internal substructures.
 * @note header is freed separately
 * @param process_t process to free
 */
void free_process(process_t *process) {
    free(process->comm);
    free(process->proc_path);
    free(process->exe);
}

char* get_process_exe(const char* proc_path){
    char* exe = (char*)malloc((strlen(proc_path) + 4) * sizeof(char));
    strcpy(exe, proc_path);
    strcat(exe, "/exe");
    return exe;
}

process_t *get_process(char *dir_name) {
    if (!is_int(dir_name)) {
        return NULL;
    }
    process_t *process = (process_t *) malloc(sizeof(process_t));
    process->proc_path = make_proc_path(dir_name);
    process->exe = get_process_exe(process->proc_path);
    process->comm = get_process_comm(process->proc_path);
    if(process->comm == NULL){
        free(process->proc_path);
        free(process->exe);
	free(process);
        return NULL;
    }
    process->pid = atoi(dir_name);
    return process;
}

/**
 * Gets all processes without calculating their checksum
 * @note Proccess checksums array is NULL
 * @return array_t of type process_t
 */
array_t *get_processes(void) {
    reset_errors;
    struct dirent *_dirent;
    DIR *dir;

    array_t *processes = array_create(128);

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

