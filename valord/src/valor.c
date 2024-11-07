#include "process.h"
#include "aassert.h"
#include "log.h"
#include "util.h"

#include <fuzzy.h>

#include <valor/db/db.h>
#include <valor/array.h>
#include <valor/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>


const char *VERSION = "0.2.5";
const char *DB_FILE = "/system/etc/valor.db";
const uint8_t IDLE_TIME = 3;
const ssize_t MATCH_THRESHOLD = 20; // minimum distance from threat on which process is not considered as threat

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
                fuzzy_hash_t hash;
                int result = fuzzy_hash_file(file, hash);
                if(!result) {
                    ssize_t score = database_check_hash(db, hash);
                    if (score < MATCH_THRESHOLD) {
                        warn("Threat with PID %d has score %zd", score);
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
