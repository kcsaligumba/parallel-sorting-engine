#include <stdio.h>
#include <stdlib.h>
#include "../include/record.h"
#include "../include/gpu.h"

void gpu_sort_records(record_t* host_arr, size_t n) {
    (void)host_arr;  // silence “unused parameter” warning
    (void)n;
    
    fprintf(stderr, "[ERROR] GPU sort requested but CUDA/Thrust not built. Rebuild with CUDA or pick --algo=qsort|radix.\n");
    exit(1);
}
