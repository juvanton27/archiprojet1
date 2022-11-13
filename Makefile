CC=gcc
CCFLAGS= -std=c11 -pedantic -Wvla -Wall -Wno-unused-variable -pthread -g -fno-unroll-loops -fno-tree-vectorize -O2

#  -mno-avx -mno-avx2 -mno-avx512f

all: server server-optim client

server : server.o 
	$(CC) $(CCFLAGS) -o server server.o utils.o

server-optim : server-optim.o 
	$(CC) $(CCFLAGS) -o server-optim server.o utils.o

client : client.o 
	$(CC) $(CCFLAGS) -o client client.o utils.o

server.o: server.c utils.o
	$(CC) -DOPTIM=0 $(CCFLAGS) -c server.c 

server-optim.o: server.c utils.o
	$(CC) -DOPTIM=1 $(CCFLAGS) -c server.c 

client.o: client.c utils.o
	$(CC) $(CCFLAGS) -c client.c

utils.o: utils.c utils.h
	$(CC) $(CCFLAGS) -c utils.c 

clean : 
	rm -f server server-optim client *.o data/*.log data/*.csv
	touch data/server.log
	touch data/response_time.log
	clear