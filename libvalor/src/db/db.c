#include <valor/db/db.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <valor/hashset.h>
#include <valor/checksum.h>

database_t* create_database(db_size_t capacity, db_size_t chunk_size){
    database_t* new = (database_t*)malloc(sizeof(database_t));
    new->chunk_size = chunk_size;
    new->modulo = capacity;
    new->chunk_set = create_hashset(capacity);
    new->name_set = create_stringset(capacity);
    new->chunks = array_create(1);
    new->names = array_create(1);
    new->version = 1;
    return new;
}

void free_database(database_t* database){
    destroy_hashset(database->chunk_set);
    destroy_stringset(database->name_set);
    array_free(database->names);
    array_free(database->chunks);
    free(database);
}

void database_add_checksums(database_t* db, array_t* checksums){
    size_t i = 0;
    checksum_t chksum = 0;
    checksum_t* chksum_ptr;
    for(; i < checksums->sz; ++i){
        chksum = *(checksum_t*)checksums->base[i];
        chksum_ptr = (checksum_t*)malloc(sizeof(chksum_ptr));
        memcpy(chksum_ptr, &chksum, sizeof(checksum_t));
        array_add(db->chunks, chksum_ptr);
        hashset_add(db->chunk_set, chksum);
    }
}

bool database_check_chunk(database_t* db, checksum_t chunk_chksum){
    return hashset_check(db->chunk_set, chunk_chksum);
}

void database_add_name(database_t* db, const char* name){
    char* name_copied = (char*) malloc((strlen(name) + 1) * sizeof(char));
    strcpy(name_copied, name);
    array_add(db->names, (void*)name_copied);
    stringset_add(db->name_set, name);
}

bool database_check_name(database_t* db, const char* name){
    return stringset_check(db->name_set, name);
}

void database_save(FILE* file, database_t* db){
    size_t i = 0;
    // Write metadata
    fwrite(&db->version, sizeof(db->version), 1, file);
    fwrite(&db->chunk_size, sizeof(db->chunk_size), 1, file);
    fwrite(&db->modulo, sizeof(db->modulo), 1, file);
    fwrite(&db->name_set->size, sizeof(db->name_set->size), 1, file);
    fwrite(&db->chunk_set->size, sizeof(db->chunk_set->size), 1, file);
    // Write all names
    for(; i < db->names->sz; ++i){
        const char* str = (char*)db->names->base[i];
        const uint32_t len = strlen(str) + 1;
        fwrite(&len, sizeof(uint32_t), 1, file);
        fwrite(str, sizeof(char), len, file);
    }
    // Write all chunks hashes
    for(i = 0; i < db->chunks->sz; ++i){
        const checksum_t* chksum_ptr = (checksum_t*)db->chunks->base[i];
        fwrite(chksum_ptr, sizeof(checksum_t), 1, file);
    }
}
