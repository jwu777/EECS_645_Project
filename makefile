
CC=gcc
CFLAGS= -ansi -pedantic -Wall -m32

cache_simulator : cache_simulator.c
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm cache_simulator