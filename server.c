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
  end = 1;
}

int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

int main(int argc, char *argv[])
{
  ssigaction(SIGINT, endServerHandler);


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
  printf("port: %i, number of threads : %i, number of bytes: %i\n", port, nbThreads, nbBytes);

  int sockfd = initSocketServer(port);
  printf("Server is listening on port %i ...\n", port);

  int integer;
  while(!end) {
    int newsockfd = saccept(sockfd);

    int nbRead = sread(newsockfd, &integer, sizeof(int));
    while(nbRead > 0) {
      printf("Integer received: %i\n", integer);

      nbRead = sread(newsockfd, &integer, sizeof(int));
    }
  }

  sclose(sockfd);
  return 0;
}
