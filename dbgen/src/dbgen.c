#include "arguments.h"
#include "message.h"

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
    argument_add_compulsory("--db", "Path to database file", ARG_STR);
    argument_add("--filename", "Threat file", ARG_STR, (argvalue) NULL, false, false);
    argument_add("--check-entry", "Check whether file in database and exit.", ARG_BOOL, argbool(false), false, true);
    argument_add("-h", "Show help", ARG_BOOL, argbool(false), false, true);

    arguments_parse(argc, argv, 1);

    FILE *db_file;
    char *db_path = (char *) argument_get("--db")->value.strValue;
    if (access(db_path, F_OK) != -1) {
        info("Reading database from %s", db_path);
        db_file = fopen(db_path, "rw");
        if (!db_file) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
        db_read(db, db_file);
        debug("-- DB INFO");
        debug("db->size=%d", db->size);
        debug("db->index_size=%d", db->index_size);
    } else {
        info("File %s do not exists, so creating new database_t", db_path);
        db_file = fopen(db_path, "w");
        if (!db_file) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
    }

#if DEBUG
    size_t i;
    for (i = 0; i < db->size; ++i) {
        printf("db->names[%zu]=%s\n", i, db->names[i]);
    }
#endif

    if (argument_check("--check-entry")) {
        if (!argument_check("--filename")) {
            die("Please, specify --filename");
        }
        FILE *threat = fopen((char *) argument_get("--filename")->value.strValue, "r");
        if (!threat) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
        checksum_t chksum;
        checksum_file(threat, &chksum);
        info("Searching for checksum %ld", chksum);
        db_process_entry_t *entry = db_get_process_entry(db, chksum);
        if (!entry) {
            info("No entry with such checksum");
        } else {
            info("Found entry");
            info("ID: %d", entry->id);
            info("Name: %s", db->names[entry->id]);
            info("Checksum: %ld", entry->checksum);
        }
        return 0;
    }
    if (argument_check("--filename")) {
        FILE *threat = fopen((char *) argument_get("--filename")->value.strValue, "r");
        if (!threat) {
            die("fopen: %s(%d)", strerror(errno), errno);
        }
        checksum_t chksum;
        checksum_file(threat, &chksum);
        db_add_process_entry(db, chksum, argument_get("--name")->value.strValue);
        fclose(threat);
    }

    if (argument_check("--checksum")) {
        db_add_process_entry(db, argument_get("--checksum")->value.intValue, argument_get("--name")->value.strValue);
    }


    assert(db);
    assert(db_file);
    fclose(db_file);
    close(open(db_path, O_RDONLY | O_WRONLY | O_TRUNC));
    db_file = fopen(db_path, "w+");
    db_save(db, db_file);
    success("Succesfully added new entry to database_t");

    arguments_finalize();
    return 0;
}
