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

void database_read(FILE* file, database_t* db){
    _Static_assert(sizeof(uint32_t) == sizeof(db->name_set->size), "Inconsistent size type for name set");
    _Static_assert(sizeof(uint32_t) == sizeof(db->chunk_set->size), "Inconsistent size type for chunk set");
    size_t i = 0;
    // Read metadata
    fread(&db->version, sizeof(db->version), 1, file);
    fread(&db->chunk_size, sizeof(db->chunk_size), 1, file);
    fread(&db->modulo, sizeof(db->modulo), 1, file);
    uint32_t names_size;
    uint32_t chunks_size;
    fread(&names_size, sizeof(uint32_t), 1, file);
    fread(&chunks_size, sizeof(uint32_t), 1, file);
    // Initialize sets
    db->name_set = create_stringset(names_size);
    db->chunk_set = create_hashset(chunks_size);
    db->chunks = array_create(chunks_size);
    db->names = array_create(names_size);
    // Read all names
    uint32_t len = 0;
    for(; i < names_size; ++i){
        fread(&len, sizeof(uint32_t), 1, file);
        char* str = (char*) malloc((len + 1) * sizeof(char));
        str[len] = (char)0;
        fread(str, sizeof(char), len, file);
        database_add_name(db, str);
    }
    // Read all chunks
    array_t* chunks = array_create(chunks_size);
    checksum_t chksum;
    for(i = 0; i < chunks_size; ++i){
        fread(&chksum, sizeof(checksum_t), 1, file);
        checksum_t* chksum_ptr = (checksum_t*) malloc(sizeof(checksum_t));
        memcpy(chksum_ptr, &chksum);
        array_add(chunks, chksum_ptr);
    }
    database_add_checksums(db, chunks);
    // Free temporate variables
    for(i = 0; i < chunks_size; ++i){
        free(chunks->base[i]);
    }
    array_free(chunks);
}

database_t* database_from_file(FILE* file){
    database_t* db = (database_t*) malloc(sizeof(database_t));
    database_read(file, db);
    return db;
}