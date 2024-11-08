#include <valor/db/db.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <valor/hashset.h>
#include <valor/ed_tree.h>
#include <valor/crc32/crc32.h>


database_t *create_database(db_size_t capacity) {
    database_t *new = (database_t *) malloc(sizeof(database_t));
    new->modulo = capacity;
    new->name_set = create_stringset(capacity);
    new->hash_tree = create_ed_tree(FUZZY_MAX_RESULT);
    new->names = array_create(1);
    new->version = 1;
    return new;
}

static void pad_hash_digest(fuzzy_hash_t hash) {
    ssize_t length = strlen(hash);
    ssize_t i = length;
    for(; i < FUZZY_MAX_RESULT; ++i) {
        hash[i] = (char)0;
    }
}

void free_database(database_t *database) {
    destroy_stringset(database->name_set);
    free_ed_tree(database->hash_tree);
    array_free(database->names);
    free(database);
}

void database_add_hash(database_t *db, fuzzy_hash_t hash) {
    pad_hash_digest(hash);
    edt_insert_string(db->hash_tree, hash);
}

void database_add_hashes(database_t *db, array_t *hashes) {
    ssize_t i = 0;
    for(; i < hashes->sz; ++i) {
        database_add_hash(db, *(fuzzy_hash_t*)hashes->base[i]);
    }
}

ssize_t database_check_hash(database_t *db, fuzzy_hash_t hash) {
    pad_hash_digest(hash);
    return edt_get_distance_to_existing(db->hash_tree, hash);
}

void database_add_name(database_t *db, const char *name) {
    char *name_copied = (char *) malloc((strlen(name) + 1) * sizeof(char));
    strcpy(name_copied, name);
    array_add(db->names, (void *) name_copied);
    stringset_add(db->name_set, name);
}

bool database_check_name(database_t *db, const char *name) {
    assert(db);
    if(name == NULL){
        fprintf(stderr, "Name passed to database_check_name is NULL!");
        return false;
    }
    return stringset_check(db->name_set, name);
}

void database_save(FILE *file, database_t *db) {
    size_t i = 0;
    // Write metadata
    fwrite(&db->version, sizeof(db->version), 1, file);
    fwrite(&db->modulo, sizeof(db->modulo), 1, file);
    fwrite(&db->name_set->size, sizeof(db->name_set->size), 1, file);
    // Write all names
    for (; i < db->names->sz; ++i) {
        const char *str = (char *) db->names->base[i];
        const uint32_t len = strlen(str) + 1;
        fwrite(&len, sizeof(uint32_t), 1, file);
        fwrite(str, sizeof(char), len, file);
    }
    // Write hashtree
    uint64_t tree_sz = tree_size(db->hash_tree);
    uint8_t* serialized_tree = serialize_ed_tree(db->hash_tree);
    fwrite(serialized_tree, sizeof(uint8_t) * tree_sz, 1, file);
    // Clean-up
    free(serialized_tree);
}

void database_read(FILE *file, database_t *db) {
    _Static_assert(sizeof(uint32_t) == sizeof(db->name_set->size), "Inconsistent size type for name set");
    assert(db);
    size_t i = 0;
    uint32_t names_size;
    // Read metadata
    fread(&db->version, sizeof(db->version), 1, file);
    fread(&db->modulo, sizeof(db->modulo), 1, file);
    fread(&names_size, sizeof(uint32_t), 1, file);
    // Initialize sets
    db->name_set = create_stringset(db->modulo);
    db->names = array_create(names_size);
    // Read all names
    uint32_t len = 0;
    for (; i < names_size; ++i) {
        fread(&len, sizeof(uint32_t), 1, file);
        char *str = (char *) malloc((len + 1) * sizeof(char));
        str[len] = (char) 0;
        fread(str, sizeof(char), len, file);
        database_add_name(db, str);
	free(str); // String copied in add_name, so we do not need anymore
    }
    // Read hashtree
    uint8_t* serialized_tree = (uint8_t*)malloc(sizeof(uint8_t) * tree_size_for_depth(FUZZY_MAX_RESULT));
    fread(serialized_tree, sizeof(uint8_t), tree_size_for_depth(FUZZY_MAX_RESULT), file);
    db->hash_tree = deserialize_ed_tree(serialized_tree);
}

database_t *database_from_file(FILE *file) {
    database_t *db = (database_t *) malloc(sizeof(database_t));
    database_read(file, db);
    return db;
}
