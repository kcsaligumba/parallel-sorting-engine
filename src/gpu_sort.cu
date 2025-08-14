// CUDA/Thrust GPU sort for large arrays of record_t by key.
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/sequence.h>
#include <cuda_runtime.h>
#include <stdint.h>
#include <stdio.h>
#include "record.h"

extern "C" void gpu_sort_records(record_t* host_arr, size_t n) {
    // copy keys and values separately to device
    thrust::device_vector<long long> d_keys(n);
    thrust::device_vector<double>    d_vals(n);
    for (size_t i = 0; i < n; ++i) {
        d_keys[i] = (long long)host_arr[i].key;
        d_vals[i] = host_arr[i].val;
    }
    thrust::sort_by_key(d_keys.begin(), d_keys.end(), d_vals.begin());
    // copy back
    for (size_t i = 0; i < n; ++i) {
        host_arr[i].key = (long long)d_keys[i];
        host_arr[i].val = (double)d_vals[i];
    }
}