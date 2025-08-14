// Parallel quicksort on arrays of record_t by key using pthreads.
// Strategy: recursive partition; spawn new threads for left/right if above cutoff and thread budget.
#define _GNU_SOURCE
#include "parallel.h"
#include "record.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    record_t* a;
    long lo, hi; // inclusive indices
    pconfig_t cfg;
    int* thread_count; // shared
    pthread_mutex_t* mtx;
} task_t;

static inline void swap_rec(record_t* a, long i, long j) {
    record_t t = a[i]; a[i] = a[j]; a[j] = t;
}

static long partition(record_t* a, long lo, long hi) {
    // median-of-three for robustness
    long mid = lo + ((hi - lo) >> 1);
    int64_t x = a[mid].key;
    long i = lo - 1;
    long j = hi + 1;
    for (;;) {
        do { ++i; } while (a[i].key < x);
        do { --j; } while (a[j].key > x);
        if (i >= j) return j;
        swap_rec(a, i, j);
    }
}

static void* qsort_worker(void* arg);

static void qsort_seq(record_t* a, long lo, long hi) {
    while (lo < hi) {
        long p = partition(a, lo, hi);
        if (p - lo < hi - (p+1)) {
            qsort_seq(a, lo, p);
            lo = p + 1;
        } else {
            qsort_seq(a, p + 1, hi);
            hi = p;
        }
    }
}

static void maybe_spawn(task_t* t, long lo, long hi) {
    if (hi - lo + 1 <= (long)t->cfg.cutoff) {
        qsort_seq(t->a, lo, hi);
        return;
    }
    // decide about threads
    pthread_t th;
    int spawn = 0;
    pthread_mutex_lock(t->mtx);
    if (*t->thread_count < t->cfg.max_threads) { 
        ++(*t->thread_count); 
        spawn = 1; 
    }
    pthread_mutex_unlock(t->mtx);

    if (spawn) {
        task_t* nt = (task_t*)malloc(sizeof(task_t));
        *nt = *t;
        nt->lo = lo; nt->hi = hi;
        pthread_create(&th, NULL, qsort_worker, nt);
        pthread_detach(th); // detached; we don't join here
    } else {
        qsort_seq(t->a, lo, hi);
    }
}

static void* qsort_worker(void* arg) {
    task_t* t = (task_t*)arg;
    long lo = t->lo, hi = t->hi;
    if (lo >= hi) goto done;

    long p = partition(t->a, lo, hi);

    // recurse: left and right; spawn one side, do the other in current thread
    long left_lo = lo, left_hi = p;
    long right_lo = p + 1, right_hi = hi;

    // choose larger side to spawn (to help load balance)
    long left_sz = left_hi - left_lo + 1;
    long right_sz = right_hi - right_lo + 1;
    if (left_sz > right_sz) {
        maybe_spawn(t, left_lo, left_hi);
        qsort_worker(&(task_t){ .a=t->a, .lo=right_lo, .hi=right_hi, .cfg=t->cfg, .thread_count=t->thread_count, .mtx=t->mtx });
    } else {
        maybe_spawn(t, right_lo, right_hi);
        qsort_worker(&(task_t){ .a=t->a, .lo=left_lo, .hi=left_hi, .cfg=t->cfg, .thread_count=t->thread_count, .mtx=t->mtx });
    }

done:
    pthread_mutex_lock(t->mtx);
    *t->thread_count -= 1;
    pthread_mutex_unlock(t->mtx);
    free(t);
    return NULL;
}

void qsort_parallel(record_t* a, size_t n, pconfig_t cfg) {
    if (n == 0) return;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    int thread_count = 1; // this thread counts as one
    task_t* root = (task_t*)malloc(sizeof(task_t));
    *root = (task_t){ .a=a, .lo=0, .hi=(long)n-1, .cfg=cfg, .thread_count=&thread_count, .mtx=&mtx };
    // run worker in current thread; it will spawn children
    qsort_worker(root);

    // Busy-wait until all workers finished (thread_count returns to 0)
    // In practice, you might use condition variables. For simplicity we spin briefly.
    for (;;) {
        pthread_mutex_lock(&mtx);
        int done = (thread_count == 0);
        pthread_mutex_unlock(&mtx);
        if (done) break;
        sched_yield();
    }
}