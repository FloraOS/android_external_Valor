#include <valor/hashtable.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

unsigned long __hash(unsigned char *str) //djb2 hash by Dan Bernstein
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

unsigned long _hash(unsigned char *str, size_t mod) { //wrapper
    return __hash(str) % mod;
}

void _hashtbl_register(hashtable_t *tbl, hashtable_node *node) {
    array_add(tbl->keys, node->key);//add key to keys storage
    array_add(tbl->values, node->value);//add value to values storage
}

hashtable_node *create_node(char *_key, void *_data) {
    hashtable_node *node = (hashtable_node *) malloc(sizeof(hashtable_node));
    node->next = NULL;
    node->value = _data;
    node->key = (char *) malloc(sizeof(char) * strlen(_key) + sizeof(char));
    strcpy(node->key, _key);
    return node;
}


hashtable_t *hashtbl_create(size_t capacity) {
    hashtable_t *_tbl = (hashtable_t *) malloc(sizeof(hashtable_t));
    _tbl->capacity = capacity;
    _tbl->sz = 0;
    _tbl->base = (hashtable_node **) malloc(sizeof(hashtable_node * ) * capacity);
    _tbl->keys = array_create(1);
    _tbl->values = array_create(1);
    bzero(_tbl->base, capacity * sizeof(hashtable_node *));
    return _tbl;
}

void hashtbl_add(hashtable_t *tbl, char *key, void *data) {
    unsigned long hash = _hash((unsigned char *) key, tbl->capacity);
    hashtable_node *node = create_node(key, data);
    if (tbl->base[hash] == NULL) { //If we have not used this hash
        tbl->base[hash] = node; //add node directly.
    } else {
        hashtable_node *iterator = tbl->base[hash]; //We used this hash so we need to attach node to last node asscoiated with this hash.
        while (iterator->next != NULL) {
            iterator = iterator->next;
        }
        iterator->next = node;
    }
    _hashtbl_register(tbl, node);
    tbl->sz += 1;
}

void *hashtbl_get(hashtable_t *tbl, char *key) {
    unsigned long hash = _hash((unsigned char *) key, tbl->capacity);
    if (tbl->base[hash] == NULL) {
        return false;
    } else {
        hashtable_node *iterator = tbl->base[hash];
        while (iterator) {
            if (!strcmp(iterator->key, key)) {
                return iterator->value;
            }
            iterator = iterator->next;
        }
        assert(iterator && !strcmp(iterator->key, key));
        return iterator->value;
    }
}

bool hashtbl_check_key(hashtable_t *tbl, char *key) {
    unsigned long hash = _hash((unsigned char *) key, tbl->capacity);
    if (tbl->base[hash] == NULL) {
        return false;
    } else {
        hashtable_node *iterator = tbl->base[hash];
        while (iterator) {
            if (!strcmp(iterator->key, key)) {
                return true;
            }
            iterator = iterator->next;
        }
        return false;
    }
}

void hashtbl_destroy(hashtable_t *tbl) {
    assert(tbl != NULL);
    int i = 0;
    hashtable_node *iterator = NULL;
    hashtable_node *next = NULL;

    for (; i < tbl->capacity; ++i) {
        iterator = tbl->base[i];
        if (!iterator) {
            continue;
        }
        next = iterator->next;
        free(iterator);
        iterator = next;
        while (iterator != NULL) {
            next = iterator->next;
            free(iterator);
            iterator = next;
        }
    }
}
