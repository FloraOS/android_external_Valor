#ifndef VALOR_HASHSET_H
#define VALOR_HASHSET_H

#include <stdint.h>

#include <valor/array.h>

typedef struct hashnode_t{
    uint32_t hash;
    struct hashnode_t* next;
} hashnode_t;

typedef struct hashset_t{
    hashnode_t** node_table;
    uint32_t capacity;
    uint32_t size;
} hashset_t;

typedef struct stringset_node_t{
    uint32_t hash;
    char* str;
    struct stringset_node_t* next;
} stringset_node_t;

typedef struct stringset_t{
    stringset_node_t** node_table;
    uint32_t capacity;
    uint32_t size;
} stringset_t;

hashset_t* create_hashset(uint32_t capacity);
void destroy_hashset(hashset_t* set);
void hashset_add(hashset_t* set, uint32_t hash);
bool hashset_check(hashset_t* set, uint32_t hash);
stringset_t* create_stringset(uint32_t capacity);
void stringset_add(stringset_t* set, const char* str);
bool stringset_check(stringset_t* set, const char* str);
void destroy_stringset(stringset_t * set);

#endif //VALOR_HASHSET_H
