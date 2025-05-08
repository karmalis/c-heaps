#include "fib_heap.h"
#include "utils.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>

struct fib_heap_stats {
    size_t size;
    size_t tree_count;
    size_t marked_node_count;
};



size_t max_rank(fib_cb_t* cb) {
    if (cb->last == NULL) {
        return 1;
    }

    size_t max_rank = 1;


    fibheap_node_t* cursor = cb->last;
    do {
        if (max_rank < cursor->rank) {
            max_rank = cursor->rank;
        }
        cursor = cursor->next;
    } while (cursor != cb->last);

    return max_rank;
}


// Allocates
fibheap_node_t* create_node(
    uint32_t key,
    uint8_t mark
) {
    fibheap_node_t * node = malloc(sizeof(fibheap_node_t));
    node->key = key;
    node->mark = mark;
    node->rank = 1;

    return node;
}

fib_cb_t* create_circular_buffer(fibheap_node_t* parent) {
    fib_cb_t* cb = malloc(sizeof(fib_cb_t));
    cb->parent = parent;
    return cb;
}

void insert_cb_after(fibheap_node_t* node, fibheap_node_t* n) {
    n->next = node->next;
    n->prev = node;
    if (node->next != NULL) {
        node->next->prev = n;
    }
    node->next = n;
}

void insert_cb_into(fib_cb_t* cb, fibheap_node_t* node) {
    if (cb->last != NULL) {
        insert_cb_after(cb->last, node);
    } else {
        cb->last = node;
    }

    // Update parent rank
    if (cb->parent != NULL) {
        cb->parent->rank = max_t(cb->parent->rank, (node->rank + 1));
    }
}


void add_node_child(fibheap_node_t* node, fibheap_node_t* child) {
    if (node->children != NULL) {
        insert_cb_into(node->children, child);
    } else {
        fib_cb_t* children = create_circular_buffer(node);
        insert_cb_into(children, child);
        node->children = children;
    }
}

void release_circular_buffer(fib_cb_t* cb);

void remove_node(fib_cb_t* cb, fibheap_node_t* node) {
    if (node->next == node) {
        cb->last = NULL;
    } else if (node->next != NULL) {
        node->next->prev = node->prev;
        if (node->prev != NULL) {
            node->prev->next = node->next;
            if (node == cb->last) {
                cb->last = node->prev;
            }
        }
    }

    // Reset parent rank if such exists
    if (cb->parent != NULL) {
        cb->parent->rank = max_rank(cb);
    }

    if (node->children != NULL) {
        release_circular_buffer(node->children);
    }

    free(node);
}

void release_circular_buffer(fib_cb_t* cb) {
    if (cb->last != NULL) {
        fibheap_node_t* cursor = cb->last;
        do {
            fibheap_node_t* next = cursor->next;
            remove_node(cb, cursor);
            cursor = cursor->next;
        } while (cursor != NULL);
    }

    free(cb);
}

// Frees
void release_fib_heap(fibheap* fheap) {
    release_circular_buffer(fheap->root);
    free(fheap);
}


// Allocates
void insert_fib(fibheap* fheap, uint32_t key) {
    fibheap_node_t* node = create_node(key, 0);

    insert_cb_into(fheap->root, node);

    if (fheap->min == NULL) {
        fheap->min = node;
        return;
    }

    if (fheap->min->key > node->key) {
        fheap->min = node;
    }
}

// Will free heap_b
void merge_fib(fibheap* heap_a, fibheap* heap_b) {
    assert(heap_a->root != NULL);
    assert(heap_b->min != NULL);

    insert_cb_into(heap_a->root, heap_b->min);
    if (heap_b->min < heap_a->min) {
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
