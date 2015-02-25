CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter

all:
	$(CC) $(CFLAGS) -o vm vm.c 
	$(CC) $(CFLAGS) -o gen gen.c 
