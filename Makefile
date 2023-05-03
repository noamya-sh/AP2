#.PHONY: all
#all: task stdinExample temp

CC=gcc
CFLAGS=-Wall -Wextra -pthread
LIBS=-L. -lCodec

all: tp

tp: ThreadPool.c queue.c heap.c queue_heap.h
	$(CC) $(CFLAGS) -o tp ThreadPool.c heap.c queue.c $(LIBS)

task:	codec.h basic_main.c
	gcc basic_main.c -L. -l Codec -o encoder

stdinExample:	stdin_main.c
		gcc stdin_main.c -L. -l Codec -o tester

.PHONY: clean
clean:
	-rm encoder tester t main
#libCodec.so 2>/dev/null
