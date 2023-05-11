#.PHONY: all
#all: task stdinExample temp

CC=gcc
CFLAGS=-Wall -Wextra -pthread
LIBS=-L. -lCodec

all: coder

coder: ThreadPool.c queue.c heap.c queue_heap.h
	$(CC) $(CFLAGS) -o coder ThreadPool.c heap.c queue.c $(LIBS)

.PHONY: clean
clean:
	-rm tp