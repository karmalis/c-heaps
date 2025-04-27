#ifndef HEAP_H_
#define HEAP_H_

#include <stdint.h>
#include <stdlib.h>

struct q_pair {
    uint32_t key;
    uint32_t val;
};

// Binary heap queue
struct bheap {
    // element array
    struct q_pair *elements;
    size_t size;
    size_t cap;
};

struct q_pair pop(struct bheap* self);
void insert(struct bheap* self, struct q_pair element);
void print_heap(struct bheap* heap, uint32_t* input);

#endif // HEAP_H_
