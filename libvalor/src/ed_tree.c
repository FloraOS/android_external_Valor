#include <valor/ed_tree.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

#include <stdio.h>

ed_tree_t* create_ed_tree(ssize_t depth){
  assert(depth > 0);
  ed_tree_t* new = (ed_tree_t*)malloc(sizeof(ed_tree_t));
  new->root = NULL;
  new->depth = depth;
  return new;
}

static inline ed_tree_node_t* make_node(const char data){
  ed_tree_node_t* new = (ed_tree_node_t*)malloc(sizeof(ed_tree_node_t));
  new->next = NULL;
  new->data = data;
  ssize_t i = 0;
  for(; i < 1 << 8 * sizeof(char); ++i){
    new->has_neighbor_at[i] = false;
  }
  return new;
}

static void edt_initialize_tree(ed_tree_t* tree, const char* bamboo){
  ssize_t i = 1;
  const char bamboo_root = bamboo[0];
  tree->root = make_node(bamboo_root);
  ed_tree_node_t* last_node = tree->root;
  for(; i < tree->depth; ++i){
    ed_tree_node_t* new_node = make_node(bamboo[i]);
    last_node->next = new_node;
    last_node = new_node;
  }
}

void edt_insert_string(ed_tree_t* tree, const char* string){
  if(!tree->root){
    edt_initialize_tree(tree, string);
    return;
  }
  ssize_t i = 0;
  ed_tree_node_t* last_node = tree->root;
  for(; i < tree->depth; ++i){
    assert(last_node);
    //printf("node=%c has neighbor at %c\n", last_node->data, string[i]);
    last_node->has_neighbor_at[string[i]] = true;
    last_node = last_node->next;
  }
}

ssize_t edt_get_distance_to_existing(ed_tree_t* tree, const char* string){
  ssize_t i = 0;
  ssize_t distance = 0;
  ed_tree_node_t* last_node = tree->root;
  for(; i < tree->depth; ++i){
    assert(last_node);
    if(last_node->data != string[i] && !last_node->has_neighbor_at[string[i]]){
      //printf("node data=%c, string[i] = %c\n", last_node->data, string[i]);
      ++distance;
    }
    last_node = last_node->next;
  }
  return distance;
}

void free_ed_tree(ed_tree_t* tree){
  ed_tree_node_t* next = tree->root;
  ed_tree_node_t* current = NULL;
  while(next){
    current = next;
    next = current->next;
    free(current);
  }
  free(tree);
}

uint8_t* serialize_ed_tree(ed_tree_t* tree){
  // The total tree size is: sizeof(depth) + depth * sizeof(data_field) + depth * (2 ^ max_possible_data_value) * sizeof(bool)
  uint64_t total_size = sizeof(ssize_t) +\
                        tree->depth * sizeof(char) +\
                        tree->depth * (1 << 8 * sizeof(char)) * sizeof(bool);
  ssize_t i = 0;
  ssize_t j = 0;
  uint8_t* serialized = (uint8_t*)malloc(sizeof(uint8_t) * total_size);
  uint8_t* serialized_start_ptr = serialized;
  memcpy(serialized, &tree->depth, sizeof(ssize_t));
  serialized += sizeof(uint64_t);
  ed_tree_node_t* last_node = tree->root;
  for(; i < tree->depth; ++i){
    assert(last_node);
    serialized[i] = last_node->data;
    // Offset of bool-array is depth + bool_array_size * i
    ssize_t offset = tree->depth * sizeof(char) + i * (1 << 8 * sizeof(char)) * sizeof(bool);
    for(j = 0; j < 1 << 8 * sizeof(char); ++j){
      /*if(last_node->has_neighbor_at[j]){
        printf("write neighbor: data=%c,neighbor=%c, offset=%zd, j=%zd, offset+j=%zd \n", last_node->data, (char)j, offset, j, offset +j);
      }*/
      serialized[offset + j] = last_node->has_neighbor_at[j];
    }
    last_node = last_node->next;
  }
  return serialized_start_ptr;
}

ed_tree_t* deserialize_ed_tree(uint8_t* data){
  uint64_t* size = (uint64_t*)data;
  data += sizeof(uint64_t);
  ed_tree_t* tree = (ed_tree_t*)malloc(sizeof(ed_tree_t));
  tree->depth = *size;
  ssize_t i = 0;
  ssize_t j = 0;
  ed_tree_node_t* last_node = NULL;
  for(; i < tree->depth; ++i){
    //printf("%d\n", data[i]);
    ed_tree_node_t* node = make_node(*((char*)(data + i * sizeof(char))));
    // Offset of bool-array is depth + bool_array_size * i
    ssize_t offset = tree->depth * sizeof(char) + i * (1 << 8 * sizeof(char)) * sizeof(bool);
    //printf("offset=%llu\n", offset);
    for(j = 0; j < 1 << 8 * sizeof(char); ++j){
      node->has_neighbor_at[j] = data[offset + j];
      /*if(node->has_neighbor_at[j]){
        printf("data=%c, neighbor=%c\n", node->data, (char)j);
      }*/
    }
    //printf("data=%c\n", node->data);
    if(!last_node){
      tree->root = node;
      last_node = node;
    }else{
      last_node->next = node;
      last_node = node;
    }
  }
  return tree;
}

ssize_t tree_size(const ed_tree_t* tree){
  return sizeof(ssize_t) +\
         tree->depth * sizeof(char) +\
         tree->depth * (1 << 8 * sizeof(char)) * sizeof(bool);
}

ssize_t tree_size_for_depth(ssize_t depth){
  return sizeof(ssize_t) +\
         depth * sizeof(char) +\
         depth * (1 << 8 * sizeof(char)) * sizeof(bool);
}