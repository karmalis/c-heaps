#ifndef FIBHEAP_H_
#define FIBHEAP_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct fibheap_node {
    uint32_t key; // Key

    size_t rank; // Rank or sometimes called degree
    uint8_t mark; // Is the marked or no (0 or 1)

    struct fibheap_node* parent;
    struct fibheap_node* child;
    struct fibheap_node* next;
    struct fibheap_node* prev;
} fibheap_node;

typedef struct fibheap {
    fibheap_node* min;

    size_t size;
    size_t max_rank;
    size_t tree_count;
    size_t marked_node_count;
} fibheap;

// Allocates
void insert_fib(fibheap* fheap, uint32_t key);
// Merges b into a, frees b
void merge_fib(fibheap* heap_a, fibheap* heap_b);
// frees min
uint32_t extract_min_fib(fibheap* fheap);

// Allocates
fibheap_node* create_node(
    uint32_t key,
    uint8_t mark,
    fibheap_node* parent,
    fibheap_node* child,
    fibheap_node* next,
    fibheap_node* prev
);

// Allocates
fibheap* create_heap_with_min(fibheap_node* min);

// Frees
void clean_fib_heap(fibheap* fheap);

#endif // FIBHEAP_H_
