#include "process.h"
#include "aassert.h"
#include "log.h"
#include "util.h"

#include <valor/db/db.h>
#include <valor/array.h>
#include <valor/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>


const char *VERSION = "0.2.4";
const char *DB_FILE = "/system/etc/valor.db";
const uint8_t IDLE_TIME = 3;

// This variable is setted to true when we are interrupted by SIGTERM
volatile sig_atomic_t need_shutdown = 0;

void sigterm_handler(int signum) {
    if(signum != SIGTERM){
        info("Received SIGTERM, setting shutdown flag");
        need_shutdown = 1;
    } else {
        warn("Got unknown signal %d", signum);
    }
}

float get_matching_k(database_t *db, array_t *chunk_checksums) {
    if(chunk_checksums == NULL){
        error("chunk_checksums is NULL");
        return 0;
    }
    size_t i = 0;
    size_t total_matches = 0;
    for (; i < chunk_checksums->sz; ++i) {
        checksum_t *chksum = (checksum_t *) chunk_checksums->base[i];
        if (database_check_chunk(db, *chksum)) {
            ++total_matches;
        }
    }
    return (float) total_matches / (float) chunk_checksums->sz;
}

int main(void) {
    info("valord(version %s, libvalor %s%s) is starting up...", VERSION, LIBVALOR_VERSION, DEBUG_STATUS_STR);
    // Register handler
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        fatal("Can not register SIGTERM handler");
        return -1;
    }
    // Load database
    database_t *db = (database_t *) malloc(sizeof(database_t));
    FILE * file = fopen(DB_FILE, "r");
    if (!file) {
        cerror("fopen");
        fatal("Failed to open %s for reading!", DB_FILE);
        return -1;
    }
    database_read(file, db);
    fclose(file);
    info("Loaded database version %d from %s", db->version, DB_FILE);

    size_t i;

    for (;;) {
        if(need_shutdown){
            break;
        }
        array_t *processes = get_processes();
        for (i = 0; i < processes->sz; ++i) {
            aassert(processes->base[i] != NULL);
            process_t proc = *(process_t *) processes->base[i];
            if (proc.comm != NULL && database_check_name(db, proc.comm)) {
                warn("Detected threat by name %s", proc.comm);
                reset_errors;
                kill(proc.pid, 9);
                if (!valor_perror("kill")) {
                    info("Sent signal 9 to %d", proc.pid);
                }
            } else if(proc.comm != NULL) {
                array_t* checksums = get_checksum(&proc, db->chunk_size);
                if(checksums != NULL) {
                    float matching_k = get_matching_k(db, checksums);
                    array_free_with_base(checksums);
                    if (matching_k > 0.2f) {
                        warn("Threat with PID %d is matching to database checksum on %.2f%%", matching_k * 100.0);
                        reset_errors;
                        kill(proc.pid, 9);
                        if (!valor_perror("kill")) {
                            info("Sent signal 9 to %d", proc.pid);
                        }
                    }
                }
            }
        }
        free_process_array(processes);
        sleep(IDLE_TIME);
    }
    info("Shutting down gracefully");
    free_database(db);
    return 0;
}
