#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multimap.h"

/* we allocate in slabs of size 64 cache sets * 8 cache lines */
#define SLAB_SIZE 512

/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/

 /* Represents a value that is associated with a given key in the multimap. */
typedef struct multimap_value {
    int value;
} multimap_value;


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;

    /* A array of the values associated with this key in the multimap. */
    multimap_value *values;

    /* size of values array */
    int size;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    struct multimap_node *left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *right_child;

    /* indicates whther or not node is head */
    int head;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
};


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found);

void free_multimap_values(multimap_value *values);
void free_multimap_node(multimap_node *node);

/*============================================================================
 * Global Variables
 *============================================================================*/
int num_nodes = 0;
multimap_node * head_node;
/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_node * alloc_mm_node() {
    multimap_node * node;

    if (num_nodes == 0 || num_nodes == SLAB_SIZE) {
        /* allocate block of memory continuously in multiples
         * of SLAB_SIZE * size of map node
         */

         node = malloc(SLAB_SIZE * sizeof(multimap_node));
         bzero(node, SLAB_SIZE * sizeof(multimap_node));
         head_node = node;
         node->head = 1;
         num_nodes = 1;
    }
    /* otherwise just make new node in new location */
    else {
        node = head_node + num_nodes;
        node->head = 0;
        num_nodes += 1;
    }

    return node;
}


/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node();
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;
                new->size = 0;
                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;
                new->size = 0;
                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(values, sizeof(multimap_value));
#endif
    free(values);

}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
    if (node->head) {
        free(node);
        num_nodes = 0;
    }
    else {
        node = NULL;
    }
}


/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_multimap_node(mm->root);
    mm->root = NULL;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm->root, key, /* create */ 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);

    /* Add the new value to the multimap node. Increase size of mem
     * using realloc call
     */

    node->values = realloc(node->values, sizeof(multimap_value)
                            * (node->size + 1));
    node->values[node->size].value = value;
    node->size += 1;
}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;

    node = find_mm_node(mm->root, key, /* create */ 0);
    if (node == NULL)
        return 0;

    /* iterative search for value */
    for (uint i = 0; i < node->size; i++) {
        if (node->values[i].value == value) {
            return 1;
        }
    }

    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    for (uint i = 0; i < node->size; i++) {
        f(node->key, node->values[i].value);
    }

    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}
