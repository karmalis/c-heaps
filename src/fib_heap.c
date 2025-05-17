#include "fib_heap.h"
#include "utils.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

struct fib_heap_stats {
    size_t size;
    size_t tree_count;
    size_t marked_node_count;
};

fibheap_node_t* min_node(fibheap_node_t* node_a, fibheap_node_t* node_b) {
    return node_a->key < node_b->key ? node_a : node_b;
}


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

fibheap_t* create_heap_with_min(fibheap_node_t* min) {
    fibheap_t* heap = malloc(sizeof(fibheap_t*));
    heap->root = create_circular_buffer(min);
    heap->max_rank = 0;
    heap->min = min;
    heap->tree_count = 1;
    heap->marked_node_count = 0;
    heap->size = 1;
}


// Allocates
fibheap_node_t* create_node(
    uint32_t key,
    uint8_t mark
) {
    fibheap_node_t * node = malloc(sizeof(fibheap_node_t));
    node->key = key;
    node->mark = mark;
    node->rank = 0;

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

// Frees b circular buffer
// TODO: Test validity
void merge_cb(fib_cb_t* cb_a, fib_cb_t* cb_b) {
    if (cb_a->last->next == NULL) {
        // Buffer A has only one child
        insert_cb_into(cb_b, cb_a->last);
        free(cb_b);
        return;
    }

    if (cb_b->last->next == NULL) {
        // Buffer B has only one child
        insert_cb_into(cb_a, cb_b->last);
        free(cb_b);
        return;
    }

    fibheap_node_t* cursor = cb_b->last;
    do {
        fibheap_node_t* next = cursor->next;
        insert_cb_after(cb_a->last, cursor);
        cursor = next;
    } while (cursor != cb_b->last);

    if (cb_a->parent != NULL) {
        cb_a->parent->rank = max_rank(cb_a) + 1;
    }

    free(cb_b);
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

void remove_cb_node(fib_cb_t* cb, fibheap_node_t* node, int release_children) {
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

    if (node->children != NULL && release_children) {
        release_circular_buffer(node->children);
    }

    if (cb->parent != NULL) {
        cb->parent->rank = max_rank(cb);
    }

    free(node);
    node = NULL;
}

void release_circular_buffer(fib_cb_t* cb) {
    if (cb->last != NULL) {
        fibheap_node_t* cursor = cb->last;
        do {
            fibheap_node_t* next = cursor->next;
            remove_cb_node(cb, cursor, 1);
            cursor = cursor->next;
        } while (cursor != NULL);
    }

    free(cb);
    cb = NULL;
}

// Frees
void release_fib_heap(fibheap_t* fheap) {
    release_circular_buffer(fheap->root);
    free(fheap);
    fheap = NULL;
}


// Allocates
void insert_fib(fibheap_t* fheap, uint32_t key, uint8_t mark) {
    fibheap_node_t* node = create_node(key, 0);
    node->mark = mark;

    insert_cb_into(fheap->root, node);

    fheap->size += 1;
    if (mark) {
        fheap->marked_node_count += 1;
    }

    if (fheap->min == NULL) {
        fheap->min = node;
        return;
    }

    if (fheap->min->key > node->key) {
        fheap->min = node;
    }
}

// Will free heap_b
void merge_fib(fibheap_t* heap_a, fibheap_t* heap_b) {
    assert(heap_a->root != NULL);
    assert(heap_b->min != NULL);

    if (heap_b->min->next == NULL) {
        // heap b has only one node at root
        insert_cb_into(heap_a->root, heap_b->min);
    } else {
        merge_cb(heap_a->root, heap_b->root);
    }

    if (heap_b->min < heap_a->min) {
        heap_a->min = heap_b->min;
    }

    heap_a->size += heap_b->size;
    heap_a->max_rank = max_t(heap_a->max_rank, heap_b->max_rank);
    heap_a->tree_count += heap_b->tree_count;
    heap_a->marked_node_count += heap_b->marked_node_count;

    free(heap_b);
    heap_b = NULL;
}

uint32_t extract_min_fib(fibheap_t* fheap) {
    fibheap_node_t* min = fheap->min;

    if (min == NULL) {
        return 0;
    }

    uint32_t result = min->key;
    fib_cb_t* min_children = min->children;

    // Remove the node from the circular buffer
    remove_cb_node(fheap->root, fheap->min, 0);

    // Pull the min node's tree up to the root level
    if (min->children != NULL) {
        merge_cb(fheap->root, min->children);
    }

    // Update for new minimum
    fibheap_node_t* cursor = fheap->root->last;
    fibheap_node_t* new_min = cursor;
    do {
        if (cursor->key < new_min->key) {
            new_min = cursor;
        }

        cursor = cursor->next;
    } while (cursor != fheap->root->last);
    cursor = NULL;
    fheap->min = new_min;

    // Recompute max rank
    // Children buffer ranks should be computed from inserts
    fheap->max_rank = max_rank(fheap->root);

    // Start consolidating
    fibheap_node_t** ranks = malloc(sizeof(fibheap_node_t*) * fheap->max_rank);
    cursor = fheap->min;
    size_t rank;
    fibheap_node_t* ranked;

    do {
consolidation_iter:
        rank = cursor->rank;
        ranked = ranks[rank];

        if (ranked == NULL) {
            // No other node of this rank yet
            // set this node to be of this rank
            ranks[rank] = cursor;
        } else if (ranked != cursor) {
            // There can be only one root of a rank
            // we consolidate the two by taking the node of a higher key
            // and placing it as a child of the lower key node
            fibheap_node_t* lower = min_node(cursor, ranked);
            fibheap_node_t* higher = (higher == cursor) ? ranked : cursor;

            // Before we insert into lower node as child, we first need to
            // update 0 order buffer
            if (higher->prev != NULL) {
                higher->prev->next = higher->next;
            }
            if (higher->next != NULL) {
                higher->next->prev = higher->prev;
            }
            // Insert higher as a child of lower node
            insert_cb_into(lower->children, higher);
            // Update ranked
            ranks[rank] = lower;

            // Reset the cursor and consolidation parameters
            cursor = fheap->min;
            memset(ranks, 0, sizeof(fibheap_node_t*) * fheap->max_rank);

            fheap->max_rank = max_rank(fheap->root);
            ranks = realloc(ranks, sizeof(fibheap_node_t*) * fheap->max_rank);

            goto consolidation_iter;
        }

        cursor = cursor->next;
    } while (cursor != fheap->min);
    free(ranks);

    return result;
}
