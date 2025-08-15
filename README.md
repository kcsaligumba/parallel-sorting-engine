
# Parallel Sorting Engine (C + pthreads + CUDA)

High-performance sorting for large financial time-series. Includes array sorts (parallel quicksort, parallel 64-bit radix, CUDA/Thrust GPU) and linked-list merge sort (serial + pthreads).

## Build
```bash
# CPU-only (qsort, radix, linked-list merge sort)
make cpu

# CPU + CUDA (enables --algo=gpu; requires NVIDIA driver + CUDA toolkit)
make cuda

# Clean
make clean
```

## Quick Start

### Synthetic Data

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

### Real CSV

Expect a header containing *timestamp* and one of {`close`, `price`, `value`}:

Example:
```bash
./sort_driver --algo=qsort --csv data/quotes.csv --value close --threads=8
./sort_driver --algo=radix --csv data/quotes.csv --value close --threads=8
./sort_driver --algo=gpu   --csv data/quotes.csv --value close
```

Each run prints a JSON line with algo, N, threads, elapsed seconds, and a correctness check (`ok:1`).

### Linked-List Merge Sort
```bash
# (optional) create input file with N integers
python scripts/generate_data.py 1048576   # writes data/input.txt in repo root if you prefer, or ./input.txt depending on your setup

# Serial linked-list merge sort
./merge_sort_serial          # reads input.txt → writes serial_output.txt

# Parallel linked-list merge sort (pthreads)
./merge_sort_parallel        # reads input.txt → writes parallel_output.txt

```

## Benchmark Script

`benchmark_.py` automates sweeps and generates plots (`time_*.png`, `speedup_*.png`) plus `results.csv`:
```bash
python scripts/benchmark.py --algs qsort,radix,gpu --sizes 1e5,1e6,1e7 --threads 1,2,4,8,16 --partial 0,90
# or benchmark on a CSV file
python scripts/benchmark.py --csv data/quotes.csv --algs qsort,radix,gpu --threads 1,4,8
```
