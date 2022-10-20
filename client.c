#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define BUFFER_SIZE 9
#define LOCAL_HOST "127.0.0.1"

/***************************************
 *              GLOBALS
***************************************/

volatile sig_atomic_t end = 0;
int sockfd;

/***************************************
 *              SERVER
***************************************/

void endClientHandler(int sig)
{
  printf("[+] Stoping client ...\n");
  end = 1;
}

// SOCKET HANDLER
int initSocketClient(char *localhost, int port)
{
  int socketfd = ssocket();
  printf("[+] Server socket created successfully.\n");
  sconnect(localhost, port, socketfd);
  printf("[+] Connected to Server.\n");
  return socketfd;
}

uint8_t *generateRequest(uint32_t index, uint32_t size)
{
  uint8_t *request = (uint8_t *)malloc(sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * size * size));
  *(uint32_t *)request = index;
  *(uint32_t *)(request + 4) = size;
  for (int i = 8; i < (size * size) + 8; i++)
  {
    request[i] = i - 8;
  }
  return request;
}

int main(int argc, char **argv)
{
  // Add Ctrl+C handling
  ssigaction(SIGINT, endClientHandler);

  // Handle the args
  char *server;
  int port;
  int keyBytes = atoi(argv[2]);
  int req = atoi(argv[4]);
  int sec = atoi(argv[6]);
  char *line = argv[7];
  char *search = ":";
  server = strtok(line, search);
  port = atoi(strtok(NULL, search));
  printf("[+] Launched with options ... size of the key : %d  -  requests/second : %d  -  time of execution (in seconds) : %d  -  ip address : %s -   tcp port : %d \n", keyBytes, req, sec, server, port);

  // Initiate the client
  sockfd = initSocketClient(server, port);

  // Initiate log file
  int fd = sopen("data/response_time.log", O_WRONLY | O_TRUNC | O_CREAT, 0644);
  char buf[BUFFER_SIZE];

  // Send requests to the server
  int s = 0;
  while (s < sec && !end)
  {
    int i = 0;
    while (i < req)
    {
      // Start duration counter
      clock_t t;
      t = clock();

      // Generate request
      size_t requestSize = sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * keyBytes * keyBytes);
      uint8_t * request = generateRequest(i, keyBytes);
      swrite(sockfd, &(*request), requestSize);

      // Retreive informations from the response
      // TO DO : Don't know the size of the encrypted file so have to take bigger but how much
      size_t responseSize = sizeof(uint8_t) + sizeof(uint32_t) + keyBytes*keyBytes*keyBytes*keyBytes*sizeof(uint8_t);
      uint8_t * response = malloc(responseSize);
      sread(sockfd, &(*response), responseSize);

      // Calculate duration
      t = clock() - t;
      int time_taken = ((double)t)/((clock_t)1); // in nanoseconde
      sprintf(buf, "%d", time_taken);
      strcat(buf, "\n");
      swrite(fd, buf, strlen(buf));

      // Dislay response from server
      // uint8_t errorCode = *(uint8_t *)response;
      // uint32_t fileSize = *(uint32_t *)(response+1);
      // uint8_t * encryptedFile = (uint8_t *) malloc(fileSize*sizeof(uint8_t));
      // encryptedFile = (uint8_t *)(response+5);
      // printf("Error code : %i, size : %i, encrypted file : ", errorCode, fileSize);
      // display(encryptedFile, fileSize);
      i++;
    }
    sleep(1);
    s++;
  }
  return EXIT_SUCCESS;
}
