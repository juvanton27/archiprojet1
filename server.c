#include "utils.h"
#include "request.h"

#define NB_FILES 1000

volatile sig_atomic_t end;
volatile uint8_t **files[NB_FILES];

void endServerHandler(int sig)
{
  printf("[+] Stoping server ...\n");
  end = 1;
}

int initSocketServer(int port, int nbThreads)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, nbThreads);
  printf("[+] Server is listening on port %i ...\n", port);
  return sockfd;
}

void initFiles(int size)
{
  printf("[+] Generating files ...\n");
  for (int i = 0; i < NB_FILES; i++)
  {
    files[i] = (volatile uint8_t **)malloc(size * sizeof(uint8_t *));
    for (int row = 0; row < size; row++)
    {
      files[i][row] = (uint8_t *)malloc(size * sizeof(uint8_t));
      for (int col = 0; col < size; col++)
      {
        files[i][row][col] = (size * (row)) + col;
      }
    }
  }
}

uint8_t **encryptFile(volatile uint8_t **file, int fileSize, uint8_t **key, int keySize)
{
  int size = fileSize / keySize;
  printf("%i\n", size);
  uint8_t **result = (uint8_t **)malloc(fileSize * sizeof(uint8_t));
  for (int i = 0; i < fileSize; i++)
  {
    result[i] = (uint8_t *)malloc(fileSize * sizeof(uint8_t));
  }
  for (int row=0; row<fileSize; row++) {
    for (int col=0; col<fileSize; col++) {
      result[row][col] += file[row][col]*key[col][row];
    }
  }
  return result;
}

int main(int argc, char *argv[])
{
  // Add Ctrl+C handling
  ssigaction(SIGINT, endServerHandler);

  // Handle the args
  int port, nbThreads, nbBytes;
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-j") == 0)
    {
      nbThreads = atoi(argv[i + 1]);
    }
    if (strcmp(argv[i], "-s") == 0)
    {
      nbBytes = atoi(argv[i + 1]);
    }
    if (strcmp(argv[i], "-p") == 0)
    {
      port = atoi(argv[i + 1]);
    }
  }
  // printf("PORT : %i, NUMBER OF THREADS : %i, NUMBER OF BYTES : %i\n", port, nbThreads, nbBytes);

  // Init files
  initFiles(nbBytes);
  display((void **)files[0], nbBytes);

  // Init the server
  int sockfd = initSocketServer(port, nbThreads);

  // Listen to client's requests
  Request request;
  while (!end)
  {
    int newsockfd = saccept(sockfd);
    int nbRead = sread(newsockfd, &request, sizeof(Request));
    while (nbRead > 0)
    {
      // display((void **) request.key, request.size);
      nbRead = sread(newsockfd, &request, sizeof(Request));
    }
  }

  // Close the connection
  sclose(sockfd);
  return 0;
}
