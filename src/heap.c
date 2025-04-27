#include "heap.h"
#include "utils.h"

#include <bits/time.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>


const uint32_t NULL_KEY = 0;

void swap(struct q_pair* a, struct q_pair* b) {
    struct q_pair temp = *a;
    *a = *b;
    *b = temp;
}

size_t left_child(size_t idx) {
    return 2*idx + 1;
}

size_t right_child(size_t idx) {
    return 2*idx + 2;
}

size_t parent(size_t idx) {
    return (idx - 1) / 2;
}


void print_heap(struct bheap* heap, uint32_t* input) {
    printf("================\n");
    printf("cap: %u size: %u\n", heap->cap, heap->size);
    for (size_t idx = 0; idx < heap->size; idx++) {
        struct q_pair elem = heap->elements[idx];
        printf("| %d |", input[elem.val]);
    }
    printf("\n");
    for (size_t idx = 0; idx < heap->size; idx++) {
        struct q_pair elem = heap->elements[idx];
        printf("| %d |", elem.key);
    }
    printf("\n");
    printf("================\n");
}

void resize(struct bheap* self) {
    size_t new_size = sizeof(struct q_pair) * (self->cap * 2);
    self->elements = realloc(self->elements, new_size);
    self->cap *= 2;
}

void insert(struct bheap* self, struct q_pair element) {
    if ((self->size) >= self->cap) {
        resize(self);
    }

    // Place the element at the bottom
    self->elements[self->size] = element;

    if (self->size > 0) {
        size_t element_at = self->size;
        while (element_at > 0) {

            if (element.key < self->elements[parent(element_at)].key) {
                break;
            }

            // Swap elements if one being inserted is of higher key
            swap(&self->elements[element_at],
                &self->elements[parent(element_at)]);

            element_at = parent(element_at);
        }
    }

    self->size++;
}

void down_heap(struct bheap* heap) {
    size_t idx = 0;

    while (1) {
        size_t old_idx = idx;

        size_t left_child_idx = left_child(idx);
        size_t right_child_idx = right_child(idx);

        uint32_t a_key = heap->elements[idx].key;

        uint32_t left_key = NULL_KEY;
        uint32_t right_key = NULL_KEY;

        if (left_child_idx < heap->size) {
            left_key = heap->elements[left_child_idx].key;
        }

        if (right_child_idx < heap->size) {
            right_key = heap->elements[right_child_idx].key;
        }

        uint32_t compare_key = max_u32(left_key, right_key);

        if (compare_key == NULL_KEY) {
            break;
        }

        size_t compare_idx = left_child_idx;

        if (compare_key == right_key) {
            compare_idx = right_child_idx;
        }

        if (a_key < compare_key) {
            swap(&heap->elements[idx], &heap->elements[compare_idx]);
            idx = compare_idx;
        }

        if (idx == old_idx) {
            break;
        }
    }
}

struct q_pair pop(struct bheap* self) {
    assert(self->size > 0);

    struct q_pair result = self->elements[0];
    swap(&self->elements[0], &self->elements[self->size-1]);
    self->size--;
    down_heap(self);

    return result;
}



