#include "utils.h"

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

uint8_t ** encryptFile(volatile uint8_t **file, int fileSize, uint8_t **key, int keySize)
{
  int size = fileSize / keySize;
  printf("%i\n", size);
  printf("File\n");
  displayAsMatrix((void **) file, fileSize);
  printf("Key\n");
  displayAsMatrix((void **) key, keySize);
  uint8_t ** result = (uint8_t ** )malloc(fileSize * sizeof(uint8_t *));
  for(int i=0; i<fileSize; i++) {
    result[i] = (uint8_t *) malloc(fileSize*sizeof(uint8_t));
  }
  for (int i = 0; i < fileSize; i++)
  {
    for (int j = 0; j < fileSize; j++)
    {
      result[i][j] = 0;
      for (int k = 0; k < keySize; k++)
      {
        result[i][j] += file[i][k] * key[k][j];
      }
    }
  }
  printf("Result\n");
  displayAsMatrix((void **) result, fileSize);
  return result;
}

int main(int argc, char *argv[])
{
  // Add Ctrl+C handling
  ssigaction(SIGINT, endServerHandler);

  // Handle the args
  int port, nbThreads, fileSize;
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-j") == 0)
    {
      nbThreads = atoi(argv[i + 1]);
    }
    if (strcmp(argv[i], "-s") == 0)
    {
      fileSize = atoi(argv[i + 1]);
    }
    if (strcmp(argv[i], "-p") == 0)
    {
      port = atoi(argv[i + 1]);
    }
  }
  // printf("PORT : %i, NUMBER OF THREADS : %i, NUMBER OF BYTES : %i\n", port, nbThreads, fileSize);

  // Init files
  initFiles(fileSize);

  // Init the server
  int sockfd = initSocketServer(port, nbThreads);

  // Listen to client's requests
  // 4 bytes : index, 4 bytes : size of the key, N*N bytes : key
  // Don't know the size of the key yet but should be smaller than the size of the file so "sizeof(uint8_t)*fileSize*fileSize"
  uint8_t *request = (uint8_t *)malloc(sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * fileSize * fileSize));
  while (!end)
  {
    int newsockfd = saccept(sockfd);
    int nbRead = sread(newsockfd, &(*request), sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * fileSize * fileSize));
    while (nbRead > 0)
    {
      // Retreive key informations from the request
      // uint32_t keyIndex = *(uint32_t *) request;
      uint32_t keySize = *(uint32_t *)(request + 4);
      uint8_t *key = malloc(sizeof(uint8_t) * keySize);
      key = (uint8_t *)(request + 8);

      // Convert array to matrix
      uint8_t ** keyMatrix = (uint8_t **) malloc(sizeof(uint8_t *)*keySize);
      for (int i = 0; i < keySize; i++)
      {
        keyMatrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * keySize);
      }
      for (int i = 0; i < keySize; i++)
      {
        for (int j = 0; j < keySize; j++)
        {
          keyMatrix[i][j] = key[(i * keySize) + j];
        }
      }

      encryptFile(files[0], fileSize, keyMatrix, keySize);

      nbRead = sread(newsockfd, *(&request), sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * fileSize * fileSize));
    }
  }

  // Close the connection
  sclose(sockfd);
  return 0;
}
