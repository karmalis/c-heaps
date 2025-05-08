#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stdlib.h>

#ifndef BILLION
#define BILLION 1000000000L
#endif

static inline void fill_random(uint32_t* array, size_t length, uint32_t max) {
    for (size_t i = 0; i < length; i++) {
        array[i] = ((uint32_t)rand() % max) + 1;
    }
}

static inline size_t max_t(size_t a, size_t b) {
    return a < b ? b : a;
}


static inline uint32_t max_u32(uint32_t a, uint32_t b) {
    return a < b ? b : a;
}


#endif // UTILS_H_
