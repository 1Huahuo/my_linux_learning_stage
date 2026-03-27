CC = gcc
CFLAGS = -Wall -g

TARGETS = test test2 test3

all: $(TARGETS)

test: test.o
	$(CC) test.o -o test

test2: test2.o
	$(CC) test2.o -o test2

test3: test3.o
	$(CC) test3.o -o test3

test.o: test.c
	$(CC) $(CFLAGS) -c test.c -o test.o

test2.o: test2.c
	$(CC) $(CFLAGS) -c test2.c -o test2.o

test3.o: test3.c
	$(CC) $(CFLAGS) -c test3.c -o test3.o

clean:
	rm -f $(TARGETS) *.o

rebuild: clean all

.PHONY: all clean rebuild
