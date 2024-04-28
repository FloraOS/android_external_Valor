#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#include <valor/checksum.h>
#include <valor/hashtable.h>
#include <valor/hashset.h>

typedef uint32_t db_size_t;

//Assert we have fixed sizes of types, so the probability of error on different architectures is less
_Static_assert(sizeof(bool) == 1, "Wrong sizeof(bool)");
_Static_assert(sizeof(uint32_t) == 4, "Wrong sizeof(uint32_t)");
_Static_assert(sizeof(checksum_t) == 4, "Wrong sizeof(checksum_t)");
_Static_assert(sizeof(char) == 1, "Wrong sizeof(char)");

typedef struct {
    db_size_t modulo;
    db_size_t chunk_size;
    stringset_t* name_set;
    hashset_t* chunk_set;
    array_t* names;
    array_t* chunks;
    uint32_t version;
} database_t;

database_t* create_database(db_size_t capacity, db_size_t chunk_size);
void free_database(database_t* database);
void database_add_checksums(database_t* db, array_t* checksums);
bool database_check_chunk(database_t* db, checksum_t chunk_chksum);
void database_add_name(database_t* db, const char* name);
bool database_check_name(database_t* db, const char* name);
void database_save(FILE* file, database_t* db);
void database_read(FILE* file, database_t* db);
database_t* database_from_file(FILE* file);

#endif 
