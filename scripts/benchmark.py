"""
Benchmark driver for CPU/GPU sorting on large-scale (synthetic or CSV) financial time series.
Usage examples:
  python benchmark_extended.py --algs qsort,radix,gpu --sizes 1e5,1e6 --threads 1,2,4,8 --partial 0,90
  python benchmark_extended.py --csv data/quotes.csv --algs qsort,radix,gpu --threads 1,4,8
"""
import argparse, json, subprocess, shlex, time, math, os, sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def run(algo, n=None, threads=8, partial=0, csv=None, value="close", exe="./sort_driver"):
    if csv:
        cmd = f'{exe} --algo={algo} --csv="{csv}" --value={value} --threads={threads}'
    else:
        cmd = f'{exe} --algo={algo} --n={n} --threads={threads} --partial={partial}'
    t0 = time.time()
    proc = subprocess.run(shlex.split(cmd), capture_output=True, text=True)
    t1 = time.time()
    if proc.returncode != 0:
        print(proc.stderr)
        raise RuntimeError(f"Command failed: {cmd}")
    out = proc.stdout.strip().splitlines()[-1]
    rec = json.loads(out)
    rec["wall"] = t1 - t0
    return rec

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--algs", default="qsort,radix,gpu")
    ap.add_argument("--sizes", default="1e5,1e6,1e7")
    ap.add_argument("--threads", default="1,2,4,8,16")
    ap.add_argument("--partial", default="0,90")
    ap.add_argument("--csv", default=None)
    ap.add_argument("--value", default="close")
    ap.add_argument("--exe", default="./sort_driver")
    args = ap.parse_args()

    algs = args.algs.split(",")
    sizes = [int(float(s)) for s in args.sizes.split(",")] if not args.csv else [None]
    threads = [int(t) for t in args.threads.split(",")]
    partials = [int(p) for p in args.partial.split(",")] if not args.csv else [0]

    rows = []
    for algo in algs:
        for p in partials:
            for n in sizes:
                for t in threads:
                    try:
                        res = run(algo, n=n, threads=t, partial=p, csv=args.csv, value=args.value, exe=args.exe)
                        res["partial"] = p
                        rows.append(res)
                        print(res)
                    except Exception as e:
                        print("ERROR:", e)

    df = pd.DataFrame(rows)
    if df.empty:
        print("No results.")
        return

    # Speedup vs threads for each algo and size
    if args.csv:
        sizes_lab = ["CSV"]
    else:
        sizes_lab = [f"{n:,}" for n in sizes]

    for algo in algs:
        for n in sizes_lab:
            d = df[(df["algo"]==algo) & ((df["n"].astype(str)==n) if n!="CSV" else (df["algo"]==algo))]
            if d.empty: continue
            base = d[d["threads"]==min(threads)]
            if base.empty: continue
            t0 = float(base["seconds"].iloc[0])
            d = d.sort_values("threads")
            plt.figure()
            plt.plot(d["threads"], t0 / d["seconds"], marker="o")
            plt.xlabel("Threads")
            plt.ylabel("Speedup (vs min threads)")
            ttl = f"{algo.upper()} speedup vs threads ({'CSV' if args.csv else n} elems)"
            plt.title(ttl)
            plt.grid(True, alpha=0.3)
            fn = f"speedup_{algo}_{'csv' if args.csv else str(d['n'].iloc[0])}.png"
            plt.savefig(fn, bbox_inches="tight")
            print("Saved", fn)

    # Time vs size for each algo at max threads
    if not args.csv:
        for algo in algs:
            d = df[(df["algo"]==algo) & (df["threads"]==max(threads))]
            if d.empty: continue
            d = d.sort_values("n")
            plt.figure()
            plt.plot(d["n"], d["seconds"], marker="o")
            plt.xscale("log"); plt.yscale("log")
            plt.xlabel("N (log)")
            plt.ylabel("Seconds (log)")
            plt.title(f"{algo.upper()} time vs N @ threads={max(threads)}")
            plt.grid(True, which="both", ls=":")
            fn = f"time_{algo}_threads{max(threads)}.png"
            plt.savefig(fn, bbox_inches="tight")
            print("Saved", fn)

    # Save raw results
    df.to_csv("results.csv", index=False)
    print("Wrote results.csv")

if __name__ == "__main__":
    main()