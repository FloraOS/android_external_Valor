#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#include <fuzzy.h>

#include <valor/hashtable.h>
#include <valor/hashset.h>
#include <valor/ed_tree.h>


typedef uint32_t db_size_t;
typedef char fuzzy_hash_t[FUZZY_MAX_RESULT];

//Assert we have fixed sizes of types, so the probability of error on different architectures is less
_Static_assert(sizeof(bool) == 1, "Wrong sizeof(bool)");
_Static_assert(sizeof(uint32_t) == 4, "Wrong sizeof(uint32_t)");
_Static_assert(sizeof(char) == 1, "Wrong sizeof(char)");


typedef struct {
    db_size_t modulo;
    stringset_t* name_set;
    array_t* names;
    ed_tree_t* hash_tree;
    uint32_t version;
} database_t;

database_t* create_database(db_size_t capacity);
void free_database(database_t* database);
void database_add_hash(database_t* db, fuzzy_hash_t hash);
ssize_t database_check_hash(database_t* db, fuzzy_hash_t hash);
void database_add_name(database_t* db, const char* name);
bool database_check_name(database_t* db, const char* name);
void database_save(FILE* file, database_t* db);
void database_read(FILE* file, database_t* db);
database_t* database_from_file(FILE* file);

#endif 
