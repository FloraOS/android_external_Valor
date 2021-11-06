#include "db.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <assert.h>

#include "../checksum.h"
#include "../config.h"

//FIXME: Add realloc's, i.e. do not assume that database is fully allocated

database* db_init(db_size_t max_size){
  database* db = malloc(sizeof(database));
  db->max_size = max_size;
  db->size = 0;
  db->index_size = DB_MODULO;
  //FIXME: For debug purposes database is fully allocated 
  db->names = (char**)malloc(sizeof(char*)*max_size);
  db->entries = (db_entry_t*)malloc(sizeof(db_entry_t)*max_size);
  db->chksum_to_entry = (uint32_t*)malloc(DB_MODULO*sizeof(uint32_t));
  db_size_t i;
  for(i = 0; i < DB_MODULO; ++i){
    db->chksum_to_entry[i] = -1;
  }
  return db;
}

void db_add_entry(database* db, checksum_t checksum, char* name){
  db->names[db->size] = (char*)malloc((1 + strlen(name))*sizeof(char));
  strcpy(db->names[db->size], name);
  db_entry_t* entry = &db->entries[db->size];
  entry->id = db->size;
  entry->checksum = checksum;
  entry->is_terminal = true;
  entry->next_node = -1;
  entry->key = checksum % DB_MODULO;
  db_size_t mod = checksum % DB_MODULO;
  uint32_t prev_id = db->chksum_to_entry[mod];
  if(prev_id >= db->size){
    db->chksum_to_entry[mod] = entry->id;
  }else{
    db_size_t cur_id = prev_id;
    while(!db->entries[cur_id].is_terminal){
      cur_id = db->entries[cur_id].next_node;
    }
    db->entries[cur_id].is_terminal = false;
    db->entries[cur_id].next_node = entry->id;
  }
  ++db->size;
}

db_entry_t* db_get_entry(database* db, checksum_t chksum){
  db_size_t mod = chksum % DB_MODULO;
  if(db->chksum_to_entry[mod] >= db->size){
    return NULL;
  }
  db_size_t cur_id = db->chksum_to_entry[mod];
  do{
    if(db->entries[cur_id].checksum == chksum){
      return &db->entries[cur_id];
    }
  }while(!db->entries[cur_id].is_terminal);
  return NULL;
}

void db_write_strings(database* db, FILE* file){
  db_size_t i;
  db_size_t len;
  for(i = 0; i < db->size; ++i){
//    debug("i=%d(before)",i);
    len = strlen(db->names[i])+1;
//    debug("i=%d(after)",i);
//    debug("name=%s", db->names[i]);
    fwrite((&len), sizeof(db_size_t), 1, file);
    fwrite(db->names[i], sizeof(char), len, file);
  }
}

void db_read_strings(database* db, FILE* file){
  db_size_t i;
  db_size_t len;
  for(i = 0; i < db->size; ++i){
    fread(&len, sizeof(db_size_t), 1, file);
    db->names[i] = (char*)malloc(sizeof(char)*len);
    fread(db->names[i], sizeof(char), len, file);
    db->names[i][len-1] = (char)0;
//    debug("Read string %s", db->names[i]);
  }
}

void db_write_entries(database* db, FILE* file){
  fwrite(db->entries, sizeof(db_entry_t), db->size, file);
}


void db_read_entries(database* db, FILE* file){
  fread(db->entries, sizeof(db_entry_t), db->size, file);
}

void db_write_index(database* db, FILE* file){
  fwrite(&db->index_size, sizeof(db_size_t), 1, file);
  fwrite(db->chksum_to_entry, sizeof(db_size_t), db->index_size, file); 
}

void db_read_index(database* db, FILE* file){
  fread(&db->index_size, sizeof(db_size_t), 1, file);
  db->chksum_to_entry = (uint32_t*)malloc(db->index_size*sizeof(uint32_t));
//  debug("Allocated chksum_to_entry");
  fread(db->chksum_to_entry, sizeof(uint32_t), db->index_size, file);
}
void db_save(database* db, FILE* file){
  assert(db);
  assert(&db->size);
  assert(&db->max_size);
  assert(file);
  fwrite(&db->size, sizeof(db_size_t), 1, file);
  fwrite(&db->max_size, sizeof(db_size_t), 1, file);
  db_write_strings(db, file);
  db_write_entries(db, file);
  db_write_index(db, file);
}

void db_read(database* db, FILE* file){
  fread(&db->size, sizeof(db_size_t), 1, file);
  fread(&db->max_size, sizeof(db_size_t), 1, file);
  db_read_strings(db, file);
  db_read_entries(db, file);
  db_read_index(db, file);
}
