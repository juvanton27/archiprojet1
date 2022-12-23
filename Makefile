CC=gcc
CCFLAGS= -std=c11 -pedantic -Wvla -Wall -Wno-unused-variable -pthread -g -fno-unroll-loops -fno-tree-vectorize -O2 #-mno-avx -mno-avx2 -mno-avx512f

files=server-queue server-optim server-float client-queue

all: $(files)

# SERVER
server-queue : server-queue.o 
	$(CC) $(CCFLAGS) -o server-queue server-queue.o utils.o

server-queue.o: server.c utils.o
	$(CC) -DOPTIM=0 $(CCFLAGS) -c server.c -o server-queue.o

# SERVER-OPTIM
server-optim : server-optim.o 
	$(CC) $(CCFLAGS) -o server-optim server-optim.o utils.o

server-optim.o: server.c utils.o
	$(CC) -DOPTIM=1 $(CCFLAGS) -c server.c -o server-optim.o

# SERVER-FLOAT
server-float : server-float.o 
	$(CC) $(CCFLAGS) -o server-float server-float.o utils.o

server-float.o: server.c utils.o
	$(CC) -DOPTIM=2 $(CCFLAGS) -c server.c -o server-float.o

# CLIENT
client-queue : client-queue.o 
	$(CC) $(CCFLAGS) -o client-queue client-queue.o utils.o -lm

client-queue.o: client.c utils.o
	$(CC) $(CCFLAGS) -c client.c -o client-queue.o

utils.o: utils.c utils.h
	$(CC) $(CCFLAGS) -c utils.c 

clean : 
	rm -f $(files) *.o data/*.log
	clear