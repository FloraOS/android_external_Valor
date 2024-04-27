#ifndef hashtable_h
#define hashtable_h

#include <sys/types.h>
#include <stdbool.h>

#include <valor/array.h>

//string hashtable_t implementation

typedef struct hashtable_node {
    char *key;
    void *value;
    struct hashtable_node *next;
} hashtable_node;

typedef struct {
    size_t sz;
    size_t capacity;
    hashtable_node **base;
    array_t *keys;
    array_t *values;
} hashtable_t;

hashtable_t *hashtbl_create(size_t capacity);

void hashtbl_add(hashtable_t *tbl, char *key, void *data);

void *hashtbl_get(hashtable_t *tbl, char *key);

bool hashtbl_check_key(hashtable_t *tbl, char *key);

void hashtbl_destroy(hashtable_t *tbl);

#endif /* hashtable_h */
