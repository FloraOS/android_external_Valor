#include "process.h"
#include "aassert.h"
#include "log.h"

#include <valor/db/db.h>
#include <valor/config.h>

#include <stdio.h>
#include <stdlib.h>


const char* VERSION = "0.1.0";
const char* DB_FILE = "/system/etc/valor.db";
const uint8_t IDLE_TIME = 3;

float get_matching_k(database_t* db, array_t* chunk_checksums){
    size_t i = 0;
    size_t total_matches = 0;
    for(; i < chunk_checksums->sz; ++i){
        checksum_t* chksum = (checksum_t*)chunk_checksums->base[i];
        if(database_check_chunk(db, *chksum)){
            ++total_matches;
        }
    }
    return (float)total_matches / (float)chunk_checksums->sz;
}

int main(void) {
    info("valord(%s, libvalor %s) is starting up...", VERSION, LIBVALOR_VERSION);
    database_t *db = (database_t*) malloc(sizeof(database_t));
    FILE *file = fopen(DB_FILE, "r");
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
        array_t *processes = get_processes(db->chunk_size);
        for (i = 0; i < processes->sz; ++i) {
            process_t proc = *(process_t *)processes->base[i];
            if(database_check_name(db, proc.comm)){
                warn("Detected threat by name %s", proc.comm);
                kill(proc.pid, 9);
                info("Sent signal 9 to %d", proc.pid);
                continue;
            }
            float matching_k = get_matching_k(db, proc.checksums);
            if(matching_k > 0.2f){
                warn("Threat with PID %d is matching to database checksum on %.2f%%", matching_k * 100.0);
                kill(proc.pid, 9);
                info("Sent signal 9 to %d", proc.pid);
            }
        }
        free_process_array(processes);
        sleep(IDLE_TIME);
    }
}
