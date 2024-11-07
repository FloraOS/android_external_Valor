#include "arguments.h"
#include "message.h"

#include <fuzzy.h>

#include <valor/config.h>
#include <valor/db/db.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, const char *argv[]) {
    arguments_begin();
    set_loglevel(LVL_FULL);
    arguments_set_usage("--db FILE [-h|--help] [--check-entry|--increment-version|--add-name NAME|--check-name NAME|--add-threat FILE|--check-threat FILE|--set-version VERSION]");
    argument_add_compulsory("--db", "Path to database file", ARG_STR);
    argument_add("--capacity", "Capacity to pre-allocate in database structures", ARG_INT,
                 (argvalue)(int64_t)1, true, false);
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
    argument_add("--set-version", "Set database version", ARG_INT,
                 argint(0), false, false);
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
        if(capacity < 0){
            die("Invalid capacity");
        }
        db = create_database(capacity);
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
        fuzzy_hash_t hash;
        int result = fuzzy_hash_file(file, hash);
        if(result){
          die("Can not hash file: %d", result);
        }
        database_add_hash(db, hash);
        info("Storing hash: %s", hash);
        success("Added hash successfully");
    }

    if(argument_check("--check-threat")){
        char* fname = argument_get("--check-threat")->value.strValue;
        FILE* file = fopen(fname, "r");
        if(!file){
            die("Can not open %s for reading", fname);
        }

        fuzzy_hash_t hash;
        int result = fuzzy_hash_file(file, hash);
        if(result){
            die("Can not hash file: %d", result);
        }
        info("Checking hash: %s", hash);
        info("Score: %llu", database_check_hash(db, hash));
    }

    if(argument_get("--increment-version")->value.boolValue){
        ++db->version;
    }

    if(argument_check("--set-version")){
        if(argument_check("--increment-version")){
            warn("Increment version has no effect when --set-version is specified");
        }
        int64_t value = argument_get("--set-version")->value.intValue;
        if(value < 0){
            die("Bad version value %d", value);
        }
        if(value == 0){
            warn("Version 0 is reserved for empty database");
        }
        db->version = value;
    }

    close(open(db_path, O_RDONLY | O_WRONLY | O_TRUNC));
    db_file = fopen(db_path, "w+");
    database_save(db_file, db);

    success("Saved revised database");

    fclose(db_file);
    arguments_finalize();
    return 0;
}
