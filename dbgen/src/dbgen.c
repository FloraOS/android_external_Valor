#include "arguments.h"
#include "message.h"

#include <valor/config.h>
#include <valor/db/db.h>
#include <valor/checksum.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, const char *argv[]) {
    arguments_begin();
    set_loglevel(LVL_FULL);
    argument_add_compulsory("--db", "Path to database file", ARG_STR);
    argument_add("--capacity", "Capacity to pre-allocate in database structures", ARG_INT,
                 (argvalue)(int64_t)1, true, false);
    argument_add("--chunk-size", "Chunk size to calculate fingerprints of threat(in bytes)", ARG_INT,
                 (argvalue)(int64_t)(1024 * 1024), true, false);
    argument_add("--filename", "Threat file", ARG_STR, (argvalue) NULL,
                 false, false);
    argument_add("--check-entry", "Check whether file in database and exit.", ARG_BOOL,
                 argbool(false), false, true);
    argument_add("--increment-version", "Flag setted to increment version of database after actions",
                 ARG_BOOL, argbool(false),false, false);
    argument_add("--add-name", "Add name of threat for by-name search", ARG_STR,
                 argstr(NULL), false, false);
    argument_add("--check-name", "Check name of threat for by name search", ARG_STR,
                 argstr(NULL), false, false);
    argument_add("--add-threat", "Add name of threat for fingerprint checksumming", ARG_STR,
                 argstr(NULL), false, false);
    argument_add("--check-threat", "Add name of threat for fingerprint checksumming", ARG_STR,
                 argstr(NULL), false, false);
    argument_add("-h", "Show help", ARG_BOOL, argbool(false),
                 false, true);

    arguments_parse(argc, argv, 1);

    FILE *db_file;
    database_t* db = NULL;
    char *db_path = (char *) argument_get("--db")->value.strValue;
    if (access(db_path, F_OK) != -1) {
        info("Reading database from %s", db_path);
        db_file = fopen(db_path, "rw");
        if (!db_file) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
        db = database_from_file(db_file);
        info("Loaded database version %d", db->version);
        fclose(db_file);
    } else {
        info("File %s do not exists, so creating new database", db_path);
        db_file = fopen(db_path, "w");
        if (!db_file) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
        int64_t capacity = argument_get("--capacity")->value.intValue;
        int64_t chunk_size = argument_get("--chunk-size")->value.intValue;
        if(chunk_size <= 0){
            die("Invalid chunk size");
        }
        if(capacity < 0){
            die("Invalid capacity");
        }
        db = create_database(capacity, chunk_size);
        fclose(db_file);
    }

    if(argument_check("--check-name")){
        char* name = argument_get("--check-name")->value.strValue;
        bool result = database_check_name(db, name);
        if(result){
            success("Name %s is present in database", name);
        } else {
            info("Name %s is not present in database", name);
        }
    }

    if(argument_check("--add-name")){
        char* name = argument_get("--add-name")->value.strValue;
        database_add_name(db, name);
    }

    if(argument_check("--add-threat")){
        char* fname = argument_get("--add-threat")->value.strValue;
        FILE* file = fopen(fname, "r");
        if(!file){
            die("Can not open %s for reading", fname);
        }
        array_t* checksums = calculate_checksum_chunks(file, db->chunk_size);
        info("Got %d checksums", checksums->sz);
        database_add_checksums(db, checksums);
        success("Added checksums successfully");
        size_t i = 0;
        for(; i< checksums->sz; ++i){
            free(checksums->base[i]);
        }
        array_free(checksums);
    }

    if(argument_check("--check-threat")){
        char* fname = argument_get("--check-threat")->value.strValue;
        FILE* file = fopen(fname, "r");
        if(!file){
            die("Can not open %s for reading", fname);
        }
        array_t* checksums = calculate_checksum_chunks(file, db->chunk_size);
        info("Got %d checksums", checksums->sz);
        size_t total_matched = 0;
        size_t i = 0;
        for(; i < checksums->sz; ++i){
            checksum_t *checksum_ptr = (checksum_t*)checksums->base[i];
            if(database_check_chunk(db, *checksum_ptr)){
                ++total_matched;
            }
        }
        success("Checked file %s. Matched %.2f%% checksums.", fname,
                100.0 * (float)total_matched / (float)checksums->sz);
    }

    if(argument_get("--increment-version")->value.boolValue){
        ++db->version;
    }

    close(open(db_path, O_RDONLY | O_WRONLY | O_TRUNC));
    db_file = fopen(db_path, "w+");
    database_save(db_file, db);

    success("Saved revised database");

    fclose(db_file);
    arguments_finalize();
    return 0;
}
