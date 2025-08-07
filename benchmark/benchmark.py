import os, time, sys, filecmp

def test_merge_serial():
    start = time.time()
    if os.system("merge_sort_serial.exe"):
        sys.exit("serial mergesort failed")
    end = time.time()
    return end - start


def test_merge_parallel():
    start = time.time()
    if os.system("merge_sort_parallel.exe"):
        sys.exit("parallel mergesort failed")
    end = time.time()
    return end - start


def test_quick_serial():
    start = time.time()
    if os.system("quick_sort_serial.exe"):
        sys.exit("serial quicksort failed")
    end = time.time()
    return end - start


def test_quick_parallel():
    start = time.time()
    if os.system("quick_sort_parallel.exe"):
        sys.exit("parallel quicksort failed")
    end = time.time()
    return end - start


if __name__ == '__main__':
    merge_serial = test_merge_serial()
    merge_parallel = test_merge_parallel()
    if filecmp.cmp("merge_sort_serial_output.txt", "merge_sort_parallel_output.txt"):
        print("Benchmark success!")
    else:
        print("Benchmark failed, parallel and serial output do not match!")
        exit(-1)
    
    print("Time taken for serial:\t\t{}\nTime taken for parallel:\t{}\nSpeedup:\t\t\t{} X".format(merge_serial, merge_parallel, round(merge_serial/merge_parallel, 2)))

    quick_serial = test_quick_serial()
    quick_parallel = test_quick_parallel()
    if filecmp.cmp("quick_sort_serial_output.txt", "quick_sort_parallel_output.txt"):
        print("Benchmark success!")
    else:
        print("Benchmark failed, parallel and serial output do not match!")
        exit(-1)
    
    print("Time taken for serial:\t\t{}\nTime taken for parallel:\t{}\nSpeedup:\t\t\t{} X".format(quick_serial, quick_parallel, round(merge_serial/merge_parallel, 2)))