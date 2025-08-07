CC=gcc
CFLAGS= -Wall -g -O0
LDFLAGS = -lpthread

# Directory structure
SRC_DIR = sorting_algorithms
INC_DIR = include

INCLUDES = $(INC_DIR)/ll.h $(INC_DIR)/merge_sort.h $(INC_DIR)/quick_sort.h

all: test merge_sort_serial merge_sort_parallel quick_sort_serial quick_sort_parallel

# Compile object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(INC_DIR)/test.o: $(INC_DIR)/test.c $(INCLUDES)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Build executables
test: $(INC_DIR)/test.o $(INC_DIR)/merge_sort.o $(INC_DIR)/quick_sort.o $(INC_DIR)/ll.o
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^

merge_sort_serial: $(SRC_DIR)/merge_sort_serial.o $(INC_DIR)/merge_sort.o $(INC_DIR)/ll.o
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^

merge_sort_parallel: $(SRC_DIR)/merge_sort_parallel.o $(INC_DIR)/merge_sort.o $(INC_DIR)/ll.o
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^ $(LDFLAGS)

quick_sort_serial: $(SRC_DIR)/quick_sort_serial.o $(INC_DIR)/quick_sort.o $(INC_DIR)/ll.o
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^

quick_sort_parallel: $(SRC_DIR)/quick_sort_parallel.o $(INC_DIR)/quick_sort.o $(INC_DIR)/ll.o
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(SRC_DIR)/*.o $(INC_DIR)/*.o test merge_sort_serial merge_sort_parallel quick_sort_serial quick_sort_parallel