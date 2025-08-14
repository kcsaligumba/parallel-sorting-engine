# Build CPU (pthreads) + GPU (CUDA) sorting engine
CC=gcc
CXX=g++
NVCC=nvcc

CFLAGS=-O3 -march=native -Wall -Wextra -pthread
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lpthread

INCLUDES=-Iinclude

CPU_SRCS=src/record.c src/timing.c src/qsort_pthreads.c src/radix64_pthreads.c src/main.c
CPU_OBJS=$(CPU_SRCS:.c=.o)

GPU_SRCS=src/gpu_sort.cu
GPU_OBJS=$(GPU_SRCS:.cu=.o)

# If you don't have CUDA installed, you can still build CPU-only target: make cpu
all: sort_driver gpu

cpu: sort_driver

sort_driver: $(CPU_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

gpu: $(GPU_OBJS)
	@echo "Built GPU object. Link happens via sort_driver when GPU used dynamically."

src/%.o: src/%.c include/%.h | include
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/main.o: src/main.c include/record.h include/parallel.h include/gpu.h include/timing.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

src/gpu_sort.o: src/gpu_sort.cu include/record.h include/gpu.h
	$(NVCC) -O3 -Xcompiler -fPIC $(INCLUDES) -c -o $@ $<

clean:
	rm -f src/*.o sort_driver

.PHONY: all clean cpu gpu