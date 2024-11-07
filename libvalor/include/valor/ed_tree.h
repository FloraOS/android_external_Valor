#ifndef ED_TREE_H
#define ED_TREE_H

#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

/*
Edit tree data structure:
 We would like to look-up similarity and compute edit distance quite quickly. The data structure for this inspired
 by BK-tree, though it is rather 256-dimensional generelazation of such tree then BK-tree itself. The aim of tree is
 to store strings(in case of valor ssdeep hashes) and quickly find Levenshtein distance. It is intentionally assumed
 that all strings are equal lengthed as ssdeep hashes have constant size.
 * For a tree node there is maximum of 256 neighbours(this why we static assert that char is exactly 1 byte)
 * If a tree node has neighbour corresponding to it this exactly means that the cost of replacement is 0
 * If a tree node has no neighbor then replacement cost is 1
 The tree itself starts from bamboo of size n and then leaves are added to this bamboo to indicate there is nodes.
 So leafed bamboo for storing words FOO, BAR and BAT would look like follows:
 /===\      /===\     /===\
 | F |---> | O  |--->| O  |
 \===/     \===/     \===/
   |         |         |
   |         |         +-----Alternatively O may be replaced by T--------+
   V         V         V                                                 V
 /===\     /===\     /===\                                             /===\
 | B |     | A |     | R |                                             | T |
 \===/     \===/     \===/                                             \===/

Note that such trees do NOT support removal unless you have a copy of everything you have putted into this tree.
To support deletion one may not just store neighbor node
*/

_Static_assert(sizeof(char) == 1, "Chars are not 1-byted");
_Static_assert(sizeof(uint8_t) == 1, "uint8_t is not 1-byted");

typedef struct __ed_tree_node_t{
  struct __ed_tree_node_t* next;
  char data;
  bool has_neighbor_at[1 << 8 * sizeof(char)]; // Yes, it is almost static. But what if computers will become 10...0 times more powerful?
 } ed_tree_node_t;

typedef struct{
  ssize_t depth; //The maximum length of string stored in ed tree
  ed_tree_node_t* root;
} ed_tree_t;

ed_tree_t* create_ed_tree(ssize_t depth);
void edt_insert_string(ed_tree_t* tree, const char* string);
ssize_t edt_get_distance_to_existing(ed_tree_t* tree, const char* string);
void free_ed_tree(ed_tree_t* tree);
uint8_t* serialize_ed_tree(ed_tree_t* tree);
ed_tree_t* deserialize_ed_tree(uint8_t* data);
ssize_t tree_size(const ed_tree_t* tree);
ssize_t tree_size_for_depth(ssize_t depth);
#endif //ED_TREE_H
