
# Parallel Sorting Engine — CPU (pthreads) + CUDA

Extension of parallel merge sort project to include **parallel quicksort**, **parallel 64‑bit radix sort**, and a **CUDA/Thrust GPU** sorter, targeting **large‑scale financial time series**.

## What’s included

- `qsort_parallel(record_t*, n, cfg)`: pthreads quicksort (array-based, in-place)
- `radix64_parallel(record_t*, n, cfg)`: pthreads stable LSD radix for 64‑bit keys
- `gpu_sort_records(record_t*, n)`: CUDA/Thrust `sort_by_key`

All algorithms operate on `record_t { int64_t key; double val; }` so you can sort by **timestamp** while carrying the price/metric along.

A simple CLI driver is provided:
```
./sort_driver --algo=[qsort|radix|gpu] [--n N | --csv FILE] [--threads T] [--partial P] [--value close]
```

## Build

### CPU-only
```bash
make cpu
```

### CPU + GPU (requires CUDA toolkit)
```bash
make
```
This builds `sort_driver` and compiles `src/gpu_sort.cu` with `nvcc`. If you don’t have CUDA installed, you can still use `make cpu`.

> If `nvcc` isn’t on your PATH, install NVIDIA CUDA or adjust `NVCC=` in the `Makefile`.

## Run (synthetic data)

- Random data, 1e6 rows, 8 threads (parallel quicksort):
```bash
./sort_driver --algo=qsort --n=1000000 --threads=8
```

- 90% pre-sorted (to test branch mispredict + partition quality):
```bash
./sort_driver --algo=qsort --n=2000000 --threads=8 --partial=90
```

- Radix (best for integer timestamps/ids):
```bash
./sort_driver --algo=radix --n=1000000 --threads=8
```

- GPU (copies to device, sorts by key with `thrust::sort_by_key`, copies back):
```bash
./sort_driver --algo=gpu --n=1000000
```

Each run prints a JSON line with algo, N, threads, elapsed seconds, and a correctness check (`ok:1`).

## Run (real CSV)

Expect a header containing *timestamp* and one of {`close`, `price`, `value`}:
```csv
timestamp,open,high,low,close,volume
1718083200000, ... 
...
```
Example:
```bash
./sort_driver --algo=qsort --csv data/quotes.csv --value close --threads=8
./sort_driver --algo=radix --csv data/quotes.csv --value close --threads=8
./sort_driver --algo=gpu   --csv data/quotes.csv --value close
```

## Benchmark script

`benchmark_extended.py` automates sweeps and generates plots (`time_*.png`, `speedup_*.png`) plus `results.csv`:
```bash
python benchmark_extended.py --algs qsort,radix,gpu --sizes 1e5,1e6,1e7 --threads 1,2,4,8,16 --partial 0,90
# or benchmark on a CSV file
python benchmark_extended.py --csv data/quotes.csv --algs qsort,radix,gpu --threads 1,4,8
```

## Integrating with your existing linked-list code

Your original merge sort works well on **singly linked lists**, but **quicksort and radix are array‑friendly** and the GPU path requires arrays. Recommended flow:

1. **Ingest**: parse CSV → array of `record_t` using `read_csv_records()`.
2. **CPU linked-list algorithms (existing)**: if you want apples-to-apples, convert array → linked list (one pass), call your list merge sort.
3. **CPU array algorithms (new)**: call `qsort_parallel` or `radix64_parallel` directly on the array.
4. **GPU**: call `gpu_sort_records` (array only).
5. **(Optional)** convert back to list if your downstream code expects it.

> If you want list‑native quicksort, implement 3‑way partitioning into {<,=,>} lists and recurse in parallel on `<` and `>` sublists; however, arrays will be significantly faster for large N due to locality.

## Notes & Tunables

- `pconfig_t.cutoff`: minimum subproblem size to spawn a new thread (default in driver is 16K). Increase to reduce threading overhead for small subarrays.
- `pconfig_t.max_threads`: soft cap for active threads during quicksort and radix passes.
- **Radix** sorts by the bit pattern of the 64-bit key; if your timestamps are signed seconds, cast to unsigned or normalize so chronological order matches unsigned ordering (the provided implementation already treats keys as unsigned during passes).
- **GPU** path uses separate key/value arrays for better coalescing. For massive data, consider **pinned host memory** and **chunked streams** to overlap H2D/D2H copies with sorting.

## Roadmap Ideas (next steps)

- **Thread pool** + work-stealing for quicksort (reduce thread create/destroy).
- **MSD radix** for nearly-sorted timestamps (early exit on shared prefixes).
- **GPU radix/merge** custom kernels for tighter control vs Thrust; consider CUB radix for peak perf.
- **I/O**: memory-map large CSVs (mmap) and parse with SIMD (simdjson) for higher ingest throughput.
- **Key-Only Sort**: build permutation on keys, then apply to a struct-of-arrays (SoA) payload.
