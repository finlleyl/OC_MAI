CC=gcc
CFLAGS=-O2 -Wall
LDFLAGS=-lzmq

all: manager node

manager: manager.o common.o
	$(CC) $(CFLAGS) -o manager manager.o common.o $(LDFLAGS)

node: node.o common.o
	$(CC) $(CFLAGS) -o node node.o common.o $(LDFLAGS)

manager.o: manager.c common.h
	$(CC) $(CFLAGS) -c manager.c

node.o: node.c common.h
	$(CC) $(CFLAGS) -c node.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

clean:
	rm -f *.o manager node
