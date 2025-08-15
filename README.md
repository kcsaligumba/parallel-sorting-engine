
# Parallel Sorting Engine (C + pthreads + CUDA)

A high-performance parallel sorting engine for **large‑scale financial time series** using:  
- **Merge Sort** (linked list, serial + pthreads)
- **Quicksort** (array, pthreads)
- **Radix Sort** (array, pthreads, 64-bit integer keys)
- **GPU Sort** (array, CUDA/Thrust)
Generate data, run comparisons, and see speedups.

## Requirements
- **CPU path:** GCC/Clang + `make` (Linux/WSL/mac) or MinGW (Windows).
- **GPU path:** NVIDIA GPU + Driver + **CUDA Toolkit** (build with `nvcc`).

## Build
```bash
# CPU-only build (qsort, radix, linked-list merge sort)
make cpu

# Full CUDA build (enables --algo=gpu; requires NVIDIA driver + CUDA toolkit)
make cuda

# Clean
make clean
```
## Quick Start

### Driver (arrays)
```bash
# Synthetic data
./sort_driver --algo=qsort --n=1000000 --threads=8
./sort_driver --algo=radix --n=2000000 --threads=8
./sort_driver --algo=gpu   --n=5000000            # needs `make cuda`

# Partially pre-sorted (to test partition behavior)
./sort_driver --algo=qsort --n=2000000 --threads=8 --partial=90

# Real CSV (needs columns: timestamp, and one of {close, price, value})
./sort_driver --algo=qsort --csv path/to/quotes.csv --value close --threads=8
./sort_driver --algo=radix --csv path/to/quotes.csv --value close --threads=8
./sort_driver --algo=gpu   --csv path/to/quotes.csv --value close
```

### Linked-List Executables
```bash
# Create input file with N integers
python scripts/generate_data.py 1048576   # writes data/input.txt in repo root if you prefer, or ./input.txt depending on your setup

# Serial linked-list merge sort
./merge_sort_serial          # reads input.txt → writes serial_output.txt

# Parallel linked-list merge sort (pthreads)
./merge_sort_parallel        # reads input.txt → writes parallel_output.txt
```

## Benchmark script

`benchmark.py` automates sweeps and generates plots (`time_*.png`, `speedup_*.png`) plus `results.csv`:
```bash
python benchmark.py --algs qsort,radix,gpu --sizes 1e5,1e6,1e7 --threads 1,2,4,8,16 --partial 0,90
# or benchmark on a CSV file
python benchmark.py --csv data/quotes.csv --algs qsort,radix,gpu --threads 1,4,8
```

## Algorithms
- **Merge** (LL): split &rarr; sort halves &rarr; merge. **Stable**. $O(n \log n)$
- **Quicksort** (array): pick pivor &rarr; partition &rarr; recurse. **In-place, not stable**. Avg $O(n \log n)$
- **Radix** (array): process numbers bye-by-byte with **counting + stable scatter. Stable**. $O(p \cdot n)$ with $p=8$ for 64-bit.
- **GPU**: copy to GPU &rarr; `thrust::sort_by_key` (massively parallel) &rarr; copy back.