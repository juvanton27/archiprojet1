#include "utils.h"

#define NB_FILES 1000

volatile sig_atomic_t end;

volatile uint8_t **files[NB_FILES];

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

  // Init files
  for(int i=0; i<NB_FILES; i++) {
    files[i] = (volatile uint8_t**) malloc(nbBytes*sizeof(uint8_t*));
    for(int row=0; row<nbBytes; row++) {
      files[i][row] = (uint8_t*) malloc(nbBytes*sizeof(uint8_t));
      for(int col=0; col<nbBytes; col++) {
        files[i][row][col] = 0;
      }
    }
  }


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
