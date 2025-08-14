#ifndef GPU_H
#define GPU_H
#include <stddef.h>
#include "record.h"
#ifdef __cplusplus
extern "C" {
#endif
// Sorts in-place by key using GPU (Thrust radix/merge under the hood).
void gpu_sort_records(record_t* host_arr, size_t n);
#ifdef __cplusplus
}
#endif
#endif