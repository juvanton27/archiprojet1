CC=gcc
CCFLAGS= -std=c11 -pedantic -Wvla -Wall -Wno-unused-variable -pthread -g -fno-unroll-loops -fno-tree-vectorize -O2

#  -mno-avx -mno-avx2 -mno-avx512f

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
	rm -f server client *.o data/*.log data/*.csv
	touch data/server.log
	touch data/response_time.log
	clear