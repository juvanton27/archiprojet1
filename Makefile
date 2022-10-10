CC=gcc
CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror

all: server client

server : server.o 
	$(CC) $(CCFLAGS) -o server server.o

client : client.o 
	$(CC) $(CCFLAGS) -o client client.o

server.o: server.c
	$(CC) $(CCFLAGS) -c server.c 

client.o: client.c
	$(CC) $(CCFLAGS) -c client.c

clean : 
	rm -f server client *.o 
	clear