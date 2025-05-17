#ifndef FIBHEAP_H_
#define FIBHEAP_H_

#include <stdint.h>
#include <stdlib.h>

struct fibheap_node;

typedef struct fib_cb {

    struct fibheap_node* parent;
    struct fibheap_node* last;

} fib_cb_t;

typedef struct fibheap_node {
    uint32_t key; // Key

    size_t rank; // Rank or sometimes called degree
    uint8_t mark; // Is the marked or no (0 or 1)

    fib_cb_t* children;

    struct fibheap_node* next;
    struct fibheap_node* prev;
} fibheap_node_t;

typedef struct fibheap {
    fibheap_node_t* min;
    fib_cb_t* root;

    size_t size;
    size_t max_rank;
    size_t tree_count;
    size_t marked_node_count;
} fibheap_t;


// Allocates
// Creates a fibheap node
fibheap_node_t* create_node(
    uint32_t key,
    uint8_t mark
);

// Circular buffer functions
fib_cb_t* create_circular_buffer(fibheap_node_t* parent);
void insert_cb_after(fibheap_node_t* node, fibheap_node_t* n);
void insert_cb_into(fib_cb_t* cb, fibheap_node_t* node);
void merge_cb(fib_cb_t* cb_a, fib_cb_t* cb_b);
void add_node_child(fibheap_node_t* node, fibheap_node_t* child);
void release_circular_buffer(fib_cb_t* cb);
void remove_cb_node(fib_cb_t* cb, fibheap_node_t* node, int release_children);

// Fib heap functions
void release_fib_heap(fibheap_t* fheap);
void insert_fib(fibheap_t* fheap, uint32_t key, uint8_t mark);


// Merges b into a, frees b
void merge_fib(fibheap_t* heap_a, fibheap_t* heap_b);
// frees min
uint32_t extract_min_fib(fibheap_t* fheap);

// Allocates
fibheap_t* create_heap_with_min(fibheap_node_t* min);

// Frees
void release_fib_heap(fibheap_t* fheap);

#endif // FIBHEAP_H_
