// Command-line driver:
//   ./sort_driver --algo=[qsort|radix|gpu] --n=1000000 --threads=8 --partial=90
// OR
//   ./sort_driver --algo=[qsort|radix|gpu] --csv=path.csv --value=close --threads=8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "../include/record.h"
#include "../include/parallel.h"
#include "../include/gpu.h"
#include "../include/timing.h"

typedef enum { ALG_QSORT, ALG_RADIX, ALG_GPU } algo_t;

int main(int argc, char** argv) {
    algo_t algo = ALG_QSORT;
    size_t n = 1000000;
    int threads = 8;
    int partial = 0;
    const char* csv = NULL;
    const char* valcol = "close";

    static struct option opts[] = {
        {"algo", required_argument, 0, 'a'},
        {"n", required_argument, 0, 'n'},
        {"threads", required_argument, 0, 't'},
        {"partial", required_argument, 0, 'p'},
        {"csv", required_argument, 0, 'c'},
        {"value", required_argument, 0, 'v'},
        {0,0,0,0}
    };
    int c;
    while ((c = getopt_long(argc, argv, "a:n:t:p:c:v:", opts, NULL)) != -1) {
        switch (c) {
            case 'a':
                if (strcmp(optarg, "qsort")==0) algo = ALG_QSORT;
                else if (strcmp(optarg, "radix")==0) algo = ALG_RADIX;
                else if (strcmp(optarg, "gpu")==0) algo = ALG_GPU;
                else { fprintf(stderr, "Unknown algo: %s\n", optarg); return 1; }
                break;
            case 'n': n = strtoull(optarg, NULL, 10); break;
            case 't': threads = atoi(optarg); break;
            case 'p': partial = atoi(optarg); break;
            case 'c': csv = optarg; break;
            case 'v': valcol = optarg; break;
            default: break;
        }
    }

    record_t* arr = NULL;
    size_t count = 0;
    if (csv) {
        arr = read_csv_records(csv, valcol, &count);
        if (!arr) { fprintf(stderr, "Failed to read CSV %s\n", csv); return 2; }
    } else {
        arr = gen_synthetic_records(n, partial, 12345);
        count = n;
    }

    pconfig_t cfg = { .cutoff = 1<<14, .max_threads = threads };
    double t0 = wall_time();
    if (algo == ALG_QSORT) {
        qsort_parallel(arr, count, cfg);
    } else if (algo == ALG_RADIX) {
        radix64_parallel(arr, count, cfg);
    } else if (algo == ALG_GPU) {
        gpu_sort_records(arr, count);
    }
    double t1 = wall_time();

    int ok = verify_sorted_by_key(arr, count);
    printf("{\"algo\":\"%s\",\"n\":%zu,\"threads\":%d,\"seconds\":%.6f,\"ok\":%d}\n",
           (algo==ALG_QSORT?"qsort":algo==ALG_RADIX?"radix":"gpu"),
           count, threads, t1 - t0, ok);

    // For quick sanity: write out a tiny sample if user sets n small
    if (!csv && count <= 50) write_csv_records("out_small.csv", arr, count);

    free(arr);
    return 0;
}