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
} hashset_t;

#endif //VALOR_HASHSET_H
