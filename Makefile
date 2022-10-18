CC=gcc
CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror

all: server client

server : server.o 
	$(CC) $(CCFLAGS) -o server server.o utils.o

client : client.o 
	$(CC) $(CCFLAGS) -o client client.o utils.o

server.o: server.c utils.o
	$(CC) $(CCFLAGS) -c server.c 

client.o: client.c utils.o
	$(CC) $(CCFLAGS) -c client.c

utils.o: utils.c utils.h
	$(CC) $(CCFLAGS) -c utils.c 

clean : 
	rm -f server client *.o bwm.csv
	clear