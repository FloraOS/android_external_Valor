#include "process.h"
#include "aassert.h"
#include "log.h"
#include "util.h"

#include <valor/db/db.h>
#include <valor/config.h>

#include <stdio.h>
#include <stdlib.h>


const char *VERSION = "0.2.0";
const char *DB_FILE = "/system/etc/valor.db";
const uint8_t IDLE_TIME = 3;

float get_matching_k(database_t *db, array_t *chunk_checksums) {
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
    info("valord(%s, libvalor %s%s) is starting up...", VERSION, LIBVALOR_VERSION, DEBUG_STATUS_STR);
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
        array_t *processes = get_processes();
        for (i = 0; i < processes->sz; ++i) {
            process_t proc = *(process_t *) processes->base[i];
            if (database_check_name(db, proc.comm)) {
                warn("Detected threat by name %s", proc.comm);
                kill(proc.pid, 9);
                if (!perror("kill")) {
                    info("Sent signal 9 to %d", proc.pid);
                }
            } else {
                set_checksum(&proc, db->chunk_size);
                float matching_k = get_matching_k(db, proc.checksums);
                if (matching_k > 0.2f) {
                    warn("Threat with PID %d is matching to database checksum on %.2f%%", matching_k * 100.0);
                    kill(proc.pid, 9);
                    if (!perror("kill")) {
                        info("Sent signal 9 to %d", proc.pid);
                    }
                }
            }
        }
        free_process_array(processes);
        sleep(IDLE_TIME);
    }
}
