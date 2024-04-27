#include <valor/hashset.h>

#include <stdlib.h>

hashnode_t* make_hashnode(uint32_t hash){
    hashnode_t* new = (hashnode_t*)malloc(sizeof(hashnode_t));
    new->hash = hash;
    new->next = NULL;
    return new;
}

hashset_t* create_hashset(uint32_t capacity){
    uint32_t i;
    hashset_t* new = (hashset_t*) malloc(sizeof(hashset_t));
    new->capacity = capacity;
    new->node_table = (hashnode_t **) malloc(capacity * sizeof(hashnode_t *));
    for(i = 0; i < capacity; ++i){
        new->node_table[i] = NULL;
    }
    return new;
}

void destroy_hashset(hashset_t* set){
    uint32_t i = 0;
    for(; i < set->capacity; ++i){
        hashnode_t* node = set->node_table[i];
        hashnode_t* next_node = NULL;
        while (node != NULL){
            next_node = node->next;
            free(node);
            node = next_node;
        }
    }
    free(set);
}

void hashset_add(hashset_t* set, uint32_t hash){
    uint32_t index = hash % set->capacity;
    hashnode_t *current_node = set->node_table[index];
    hashnode_t *last_node = NULL;
    while (current_node != NULL) {
        if (current_node->hash == hash) {
            return;
        }
        last_node = current_node;
        current_node = current_node->next;
    }
    hashnode_t* new_node = make_hashnode(hash);
    if (last_node == NULL) {
        set->node_table[index] = new_node;
    } else {
        last_node->next = new_node;
    }
}

bool hashset_check(hashset_t* set, uint32_t hash){
    uint32_t index = hash % set->capacity;
    hashnode_t* current_node = set->node_table[index];
    while (current_node != NULL){
        if(current_node->hash == hash){
            return true;
        }
    }
    return false;
}
