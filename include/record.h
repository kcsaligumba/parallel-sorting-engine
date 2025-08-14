#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// A simple key-value record for financial time series.
// key: usually a UNIX timestamp in milliseconds or nanoseconds
// val: e.g., price (double) or any numeric metric
typedef struct {
    int64_t key;
    double  val;
} record_t;

// Read CSV with header containing at least "timestamp" and one of {"close","price","value"}.
// timestamp expected as integer (ms/ns/seconds).
// On success returns an allocated array of records and sets *out_n.
// The caller must free() the returned pointer.
record_t* read_csv_records(const char* path, const char* value_col, size_t* out_n);

// Generate synthetic records. If pct_sorted in [0,100], will create partially-sorted data:
// the leading pct_sorted% of the array is sorted by key, the rest is random.
record_t* gen_synthetic_records(size_t n, int pct_sorted, unsigned int seed);

// Verify array is sorted by key (non-decreasing). Returns 1 if sorted, 0 otherwise.
int verify_sorted_by_key(const record_t* arr, size_t n);

// Utility: write array to a CSV (timestamp,value) for sanity checks.
int write_csv_records(const char* path, const record_t* arr, size_t n);

#ifdef __cplusplus
}
#endif

#endif // RECORD_H