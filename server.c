#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "utils.h"

#define BACKLOG 5

volatile sig_atomic_t end;

void endServerHandler(int sig) {
  printf("[+] Stoping server ...\n");
  end = 1;
}

int initSocketServer(int port, int nbThreads)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, nbThreads);
  return sockfd;
}

int main(int argc, char *argv[])
{
  // Add Ctrl+C handling
  ssigaction(SIGINT, endServerHandler);

  // Handle the args
  int port, nbThreads, nbBytes;
  for(int i=0; i<argc; i++) {
    if(strcmp(argv[i], "-j") == 0) {
      nbThreads = atoi(argv[i+1]);
    } 
    if(strcmp(argv[i], "-s") == 0) {
      nbBytes = atoi(argv[i+1]);
    }
    if(strcmp(argv[i], "-p") == 0) {
      port = atoi(argv[i+1]);
    }
  }
  printf("PORT : %i, NUMBER OF THREADDS : %i, NUMBER OF BYTES : %i\n", port, nbThreads, nbBytes);

  // Init the server
  int sockfd = initSocketServer(port, nbThreads);
  printf("[+] Server is listening on port %i ...\n", port);

  int integer;
  while(!end) {
    int newsockfd = saccept(sockfd);

    // Listen to client's requests
    int nbRead = sread(newsockfd, &integer, sizeof(int));
    while(nbRead > 0) {
      printf("Integer received: %i\n", integer);
      nbRead = sread(newsockfd, &integer, sizeof(int));
    }
  }

  // Close the connection
  sclose(sockfd);
  return 0;
}
