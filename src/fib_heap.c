#include "fib_heap.h"
#include "utils.h"
#include <stdint.h>


// Allocates
fibheap_node* create_node(
    uint32_t key,
    uint8_t mark,
    fibheap_node* parent,
    fibheap_node* child,
    fibheap_node* next,
    fibheap_node* prev
) {
    fibheap_node* node = malloc(sizeof(fibheap_node));
    node->key = key;
    node->mark = mark;
    node->rank = 1;

    if (child != NULL) {
        node->rank = child->rank + 1;
    }
    child->parent = node;

    node->parent = NULL;
    node->child = child;
    node->prev = prev;
    node->next = next;

    return node;
}

// Allocates
void insert_fib(fibheap* fheap, uint32_t key) {
    fibheap_node* node = malloc(sizeof(fibheap_node));

    node->key = key;
    node->mark = 0;
    node->rank = 1;

    if (fheap->min == NULL) {
        fheap->min = node;
        return;
    }

    if (fheap->min->next != NULL) {
        node->next = fheap->min->next;
    }

    fheap->min->next = node;
    node->prev = fheap->min;

    if (fheap->min->key > node->key) {
        fheap->min = node;
    }
}


void merge_fib(fibheap* heap_a, fibheap* heap_b) {
    if (heap_b->min == NULL) {
        free(heap_b);
        return;
    }

    if (heap_a->min == NULL) {
        heap_a->min = heap_b->min;
        free(heap_b);
    }

    if (heap_a->min->next != NULL) {
        heap_b->min->next = heap_a->min->next;
    }

    if (heap_b->min->prev != NULL) {
        heap_a->min->next = heap_b->min->prev;
        heap_b->min->prev->prev = heap_a->min;
    } else {
        heap_a->min->next = heap_b->min;
        heap_b->min->prev = heap_a->min;
    }

    if (heap_b->min->key < heap_a->min->key) {
        heap_a->min = heap_b->min;
    }

    heap_a->size += heap_b->size;
    heap_a->max_rank = max_t(heap_a->max_rank, heap_b->max_rank);
    heap_a->tree_count += heap_b->tree_count;
    heap_a->marked_node_count += heap_b->marked_node_count;

    free(heap_b);
}

uint32_t extract_min_fib(fibheap* fheap) {
    fibheap_node* min = fheap->min;

    if (min == NULL) {
        return 0;
    }

    uint32_t result = min->key;

    // Pull the min node's tree up to the root level
    if (min->child != NULL && min->prev != min->next) {
        fibheap_node* prev = min->child;
        if (min->child->prev != NULL) {
            prev = min->child->prev;
        }
        fibheap_node* next = min->child;
        if (min->child->next != NULL) {
            next = min->child->next;
        }

        if (prev != next) {
            min->prev->next = prev;
            min->next->prev = next;
        } else {
            min->prev->next = min->child;
            min->child->prev = min->prev;

            min->next->prev = min->child;
            min->child->next = min->next;
        }
    } else if (min->prev != min->next) {
        min->prev->next = min->next;
        min->next->prev = min->prev;
    }

    // Check for a new minimum
    fibheap_node* new_min = min->next;
    fibheap_node* cursor = min->next;
    fibheap_node* stop = min->next;
    size_t idx = 0;
    while (cursor != stop || idx != 0) {

        if (cursor->key < new_min->key && cursor != min) {
            new_min = cursor;
        }

        idx++;
        cursor = cursor->next;
    }

    // Consolidation part
    // Cursor should be at the "start"
    fibheap_node** ranks = malloc(sizeof(fibheap_node) * fheap->max_rank);
    idx = 0;
    while (cursor != stop || idx != 0) {
        size_t rank = cursor->rank;
        fibheap_node* ranked = ranks[rank];
        if (ranked == NULL) {
            // We set the rank to contain a pointer
            ranked = cursor;
        } else {
            // Link higher into lower
            fibheap_node* parent = NULL;
            fibheap_node* child = NULL;

            // Actual consolidation
            // Select the lower key node
            if (cursor->key < ranked->key) {
                parent = cursor;
                child = ranked;
            } else {
                parent = ranked;
                child = cursor;
            }

            // we also need to make sure the new child's old connections are
            // reconnected with the child's apropriate neighbors
            if (child->prev != NULL && child->prev != child->next) {
                child->prev->next = child->next;
                child->next->prev = child->prev;
            }

            // Easy selection if we're merging with an empty tree
            if (parent->child == NULL) {
                parent->child = child;
            } else {
                // parent has a child, we have to insert the other child into the ring
                // buffer as the previous
                if (parent->child->prev != NULL) {
                    // parent's child has a previous item
                    parent->child->prev->next = child;
                    child->next = parent->child;
                    child->prev = parent->child->prev;
                } else {
                    // parent has a child but no previous or next item
                    parent->child->prev = child;
                    child->next = parent->child;
                    child->prev = parent->child;
                }
            }

            // Consolidation has happened and topology changed
            // We need to restart the scan
            parent->mark += child->mark;
            parent->rank = max_t(parent->rank, child->rank) + 1;

            cursor = new_min->next;
            idx = 0;
            continue;
        }

        idx++;
        cursor = cursor->next;
    }

    // Release the minimum
    free(min);
    // Release the ranks
    free(ranks);

    return result;
}
