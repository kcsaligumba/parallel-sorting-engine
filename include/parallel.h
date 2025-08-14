#ifndef PARALLEL_H
#define PARALLEL_H
#include <stddef.h>
#include <stdint.h>
#include "record.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t cutoff;     // min subproblem size to spawn threads
    int    max_threads;// soft cap on threads; recursion spawns up to this
} pconfig_t;

// Parallel quicksort (array, in-place) by key.
void qsort_parallel(record_t* a, size_t n, pconfig_t cfg);

// Parallel LSD radix sort (stable) for 64-bit keys, sorts by key and permutes values accordingly.
// Requires an auxiliary buffer of size n (allocated internally).
void radix64_parallel(record_t* a, size_t n, pconfig_t cfg);

#ifdef __cplusplus
}
#endif
#endif