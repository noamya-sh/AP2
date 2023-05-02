#.PHONY: all
#all: task stdinExample temp

#gg: main.cpp
#	g++ -std=c++17 main.cpp -lpthread -L. -lCodec -o main
#	#g++ -std=c++17 -Wl,-V -L. -lCodec -Wall -Wextra main.cpp -o main
##	g++ -o main main.cpp -I/mnt/c/Users/Public/untitled2/Codec -L/mnt/c/Users/Public/untitled2 -lCodec
CC=gcc
CFLAGS=-Wall -Wextra -pthread
LIBS=-L. -lCodec

all: temp

temp: temp.c queue.c queue.h
	$(CC) $(CFLAGS) -o temp temp.c queue.c $(LIBS)

task:	codec.h basic_main.c
	gcc basic_main.c -L. -l Codec -o encoder

stdinExample:	stdin_main.c
		gcc stdin_main.c -L. -l Codec -o tester

.PHONY: clean
clean:
	-rm encoder tester t main
#libCodec.so 2>/dev/null
