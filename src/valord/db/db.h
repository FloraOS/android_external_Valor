#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#include "../checksum.h"

typedef uint32_t db_size_t;

typedef struct{
  uint32_t id;
  checksum_t checksum;
  uint32_t key;
  bool is_terminal;
  uint32_t next_node;
} db_entry_t;

typedef struct{
  db_entry_t* entries;
  uint32_t* chksum_to_entry;
  db_size_t index_size;
  char** names;
  db_size_t max_size;
  db_size_t size;
} database;

database* db_init(db_size_t max_size);
void db_add_entry(database* db, checksum_t checksum, char* name);
db_entry_t* db_get_entry(database* db, checksum_t chksum);
void db_save(database* db, FILE* file);
void db_read(database* db, FILE* file);

#endif 
