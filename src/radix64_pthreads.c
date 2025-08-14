// Parallel stable LSD radix sort for 64-bit unsigned keys (treat int64 as unsigned for order by bit pattern).
// We sort by key and carry val along.
#define _GNU_SOURCE
#include "../include/parallel.h"
#include "../include/record.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    record_t* src;
    record_t* dst;
    size_t     n;
    size_t     start;
    size_t     end;
    int        shift; // which byte (0..7) * 8
    size_t*    global_counts; // 256-sized
    size_t*    local_hist;    // per-thread histogram (size 256)
} radix_chunk_t;

static void* count_pass(void* arg) {
    radix_chunk_t* c = (radix_chunk_t*)arg;
    memset(c->local_hist, 0, 256 * sizeof(size_t));
    for (size_t i = c->start; i < c->end; ++i) {
        uint64_t k = (uint64_t)c->src[i].key;
        unsigned b = (unsigned)((k >> c->shift) & 0xFFu);
        c->local_hist[b]++;
    }
    return NULL;
}

static void* scatter_pass(void* arg) {
    radix_chunk_t* c = (radix_chunk_t*)arg;
    // compute starting offsets for this chunk from global_counts (which is now prefix sums)
    size_t offsets[256];
    for (int b = 0; b < 256; ++b) {
        offsets[b] = c->global_counts[b];
        c->global_counts[b] += c->local_hist[b];
    }
    for (size_t i = c->start; i < c->end; ++i) {
        uint64_t k = (uint64_t)c->src[i].key;
        unsigned b = (unsigned)((k >> c->shift) & 0xFFu);
        size_t pos = offsets[b]++;
        c->dst[pos] = c->src[i];
    }
    return NULL;
}

void radix64_parallel(record_t* a, size_t n, pconfig_t cfg) {
    if (n == 0) return;
    record_t* tmp = (record_t*)malloc(n * sizeof(record_t));
    if (!tmp) { perror("malloc"); exit(1); }

    int T = cfg.max_threads > 0 ? cfg.max_threads : 1;
    if (T > 64) T = 64; // sane cap
    pthread_t* threads = (pthread_t*)malloc(T * sizeof(pthread_t));
    radix_chunk_t* chunks = (radix_chunk_t*)malloc(T * sizeof(radix_chunk_t));
    size_t** locals = (size_t**)malloc(T * sizeof(size_t*));

    size_t chunk = (n + T - 1) / T;

    for (int pass = 0; pass < 8; ++pass) {
        int shift = pass * 8;

        // allocate local hists
        for (int t = 0; t < T; ++t) {
            locals[t] = (size_t*)calloc(256, sizeof(size_t));
            size_t s = t * chunk;
            size_t e = s + chunk; if (e > n) e = n;
            chunks[t] = (radix_chunk_t){ .src=a, .dst=tmp, .n=n, .start=s, .end=e, .shift=shift, .global_counts=NULL, .local_hist=locals[t] };
        }

        // counting phase
        for (int t = 0; t < T; ++t) pthread_create(&threads[t], NULL, count_pass, &chunks[t]);
        for (int t = 0; t < T; ++t) pthread_join(threads[t], NULL);

        // reduce locals to global histogram
        size_t global[256]; memset(global, 0, sizeof(global));
        for (int t = 0; t < T; ++t) {
            for (int b = 0; b < 256; ++b) global[b] += chunks[t].local_hist[b];
        }
        // prefix sums
        size_t acc = 0;
        for (int b = 0; b < 256; ++b) { size_t c = global[b]; global[b] = acc; acc += c; }

        // scatter phase: give each chunk a pointer to a private copy of running offsets
        for (int t = 0; t < T; ++t) {
            chunks[t].global_counts = (size_t*)malloc(256 * sizeof(size_t));
            memcpy(chunks[t].global_counts, global, 256*sizeof(size_t));
            pthread_create(&threads[t], NULL, scatter_pass, &chunks[t]);
        }
        for (int t = 0; t < T; ++t) pthread_join(threads[t], NULL);

        // free locals
        for (int t = 0; t < T; ++t) { free(locals[t]); free(chunks[t].global_counts); }

        // swap buffers
        record_t* tmp2 = a; a = tmp; tmp = tmp2;
    }

    // If the final result is in tmp (odd number of passes), copy back
    if ((8 % 2) == 1) memcpy(tmp, a, n * sizeof(record_t));

    free(tmp);
    free(locals);
    free(chunks);
    free(threads);
}