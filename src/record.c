#define _GNU_SOURCE
#include "record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

static int64_t atoll_fallback(const char* s) {
    // robust-ish parse for int64
    char* end = NULL;
    long long v = strtoll(s, &end, 10);
    if (end == s) return 0;
    return (int64_t)v;
}

record_t* read_csv_records(const char* path, const char* value_col, size_t* out_n) {
    *out_n = 0;
    FILE* f = fopen(path, "r");
    if (!f) return NULL;

    // read header
    char buf[1<<16];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return NULL; }

    // tokenize header
    int ts_idx = -1, val_idx = -1;
    int idx = 0;
    char* save = NULL;
    for (char* tok = strtok_r(buf, ",\r\n", &save); tok; tok = strtok_r(NULL, ",\r\n", &save), ++idx) {
        if (strcasecmp(tok, "timestamp") == 0 || strcasecmp(tok, "time") == 0 || strcasecmp(tok, "date") == 0) ts_idx = idx;
        if (strcasecmp(tok, value_col) == 0) val_idx = idx;
    }
    if (ts_idx < 0 || val_idx < 0) { fclose(f); return NULL; }

    size_t cap = 1<<20; // start with capacity for ~1M rows; will grow as needed
    record_t* arr = (record_t*)malloc(cap * sizeof(record_t));
    if (!arr) { fclose(f); return NULL; }

    size_t n = 0;
    while (fgets(buf, sizeof(buf), f)) {
        // skip empty
        if (buf[0] == '\n' || buf[0] == '\r' || buf[0] == 0) continue;
        // copy line to avoid strtok_r modifying original
        char line[1<<16];
        strncpy(line, buf, sizeof(line)-1);
        line[sizeof(line)-1] = 0;

        int i = 0;
        char* save2 = NULL;
        int64_t ts = 0;
        double  vv = 0.0;
        for (char* tok = strtok_r(line, ",\r\n", &save2); tok; tok = strtok_r(NULL, ",\r\n", &save2), ++i) {
            if (i == ts_idx) ts = atoll_fallback(tok);
            else if (i == val_idx) vv = atof(tok);
        }
        if (n == cap) {
            cap = (cap * 3) / 2 + 1;
            record_t* tmp = (record_t*)realloc(arr, cap * sizeof(record_t));
            if (!tmp) { free(arr); fclose(f); return NULL; }
            arr = tmp;
        }
        arr[n].key = ts;
        arr[n].val = vv;
        ++n;
    }
    fclose(f);
    *out_n = n;
    return arr;
}

record_t* gen_synthetic_records(size_t n, int pct_sorted, unsigned int seed) {
    record_t* arr = (record_t*)malloc(n * sizeof(record_t));
    if (!arr) return NULL;
    srand(seed);
    for (size_t i = 0; i < n; ++i) {
        arr[i].key = (int64_t)rand() * 48271LL + (int64_t)i; // mix index to avoid degeneracy
        arr[i].val = (double)rand() / (double)RAND_MAX;
    }
    // partially sort first pct_sorted%
    if (pct_sorted > 0) {
        size_t k = (size_t)((double)pct_sorted * 0.01 * (double)n);
        // simple insertion sort for k (k may be large - but this is only for dataset shaping)
        for (size_t i = 1; i < k; ++i) {
            record_t tmp = arr[i];
            size_t j = i;
            while (j > 0 && arr[j-1].key > tmp.key) { arr[j] = arr[j-1]; --j; }
            arr[j] = tmp;
        }
    }
    return arr;
}

int verify_sorted_by_key(const record_t* a, size_t n) {
    for (size_t i = 1; i < n; ++i) if (a[i-1].key > a[i].key) return 0;
    return 1;
}

int write_csv_records(const char* path, const record_t* arr, size_t n) {
    FILE* f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "timestamp,value\n");
    for (size_t i = 0; i < n; ++i) fprintf(f, "%lld,%.10f\n", (long long)arr[i].key, arr[i].val);
    fclose(f);
    return 0;
}