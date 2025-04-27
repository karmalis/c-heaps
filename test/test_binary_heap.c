#include "../src/heap.h"
#include "../src/utils.h"

#include <stdio.h>
#include <time.h>
#include <errno.h>


#ifndef BILLION
#define BILLION 1000000000L
#endif
/* Benchmarks
 *
 * Some basic benchmakrs for the queue
 *
 */

void benchmark_individual_inserts_bheap(size_t N) {
    uint32_t input[N];
    fill_random(&input[0], N, N);

    struct bheap heap;
    heap.cap = N;
    heap.size = 0;
    heap.elements = malloc(N * sizeof(struct q_pair));

    uint64_t metrics[N];
    uint64_t total = 0.0;

    struct timespec start, end;
    uint64_t diff = 0;
    for (size_t idx = 0; idx < N; idx++) {
        uint32_t in = input[idx];
        struct q_pair element = {};
        element.key = in;
        element.val = idx;


        clock_gettime(CLOCK_MONOTONIC, &start);
        insert(&heap, element);
        clock_gettime(CLOCK_MONOTONIC, &end);

        diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

        metrics[idx] = diff;
        total += metrics[idx];
    }

    double avg = (double) total / (double) N;

    printf("Total insert for %lu elements is: %llu microseconds\n", N, (long long unsigned int) total / 1000);
    printf("Average insert for %lu elements is: %f nanoseconds\n", N, avg);

    free(heap.elements);
}

void benchmark_individual_pop_bheap(size_t N) {
    uint32_t input[N];
    fill_random(&input[0], N, N);

    struct bheap heap;
    heap.cap = N;
    heap.size = 0;
    heap.elements = malloc(N * sizeof(struct q_pair));

    uint64_t metrics[N];
    uint64_t total = 0.0;

    for (size_t idx = 0; idx < N; idx++) {
        uint32_t in = input[idx];
        struct q_pair element = {};
        element.key = in;
        element.val = idx;

        insert(&heap, element);
    }

    struct timespec start, end;
    uint64_t diff = 0;
    for (size_t idx = 0; idx < N; idx++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        struct q_pair popped = pop(&heap);
        clock_gettime(CLOCK_MONOTONIC, &end);

        diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

        metrics[idx] = diff;
        total += metrics[idx];
    }

    double avg = (double) total / (double) N;

    printf("Total pop for %lu elements is: %llu microseconds\n", N, (long long unsigned int) (total / 1000));
    printf("Average pop for %lu elements is: %f ns\n", N, (avg / 1));

    free(heap.elements);
}

int main(int argc, char** argv) {
    size_t n;
    char* endptr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;

    unsigned long ul = strtoul(argv[1], &endptr, 10);
    if (endptr == argv[1] || *endptr != '\0') {
        fprintf(stderr, "Invalid number format: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    n = (size_t)ul;

    srand(time(NULL));

    benchmark_individual_inserts_bheap(n);
    benchmark_individual_pop_bheap(n);


  return EXIT_SUCCESS;
}
