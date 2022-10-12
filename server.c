#include "utils.h"

#define NB_FILES 1000

/***************************************
 *              GLOBALS
***************************************/

volatile sig_atomic_t end;
volatile uint8_t **files[NB_FILES];


/***************************************
 *              SERVER
***************************************/

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

/***************************************
 *              FILES
***************************************/

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
  uint8_t ** result = (uint8_t **) malloc(fileSize * sizeof(uint8_t *));
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
  // TO DO : Fix encryption
  return result;
}


/***************************************
 *              MATRIX
***************************************/

uint8_t ** arrayToMatrix(void * array, int size) {
  uint8_t * arr = (uint8_t *) array;
  uint8_t ** matrix = (uint8_t **) malloc(sizeof(uint8_t *)*size);
  for (int i = 0; i < size; i++)
  {
    matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * size);
  }
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      matrix[i][j] = arr[(i * size) + j];
    }
  }
  return matrix;
}

uint8_t * matrixToArray(void ** matrix, int size) {
  uint8_t ** mat = (uint8_t **) matrix;
  uint8_t * array = (uint8_t *) malloc(sizeof(uint8_t)*size*size);
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      array[(i*size)+j] = mat[i][j];
    }
  }
  return array;
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
  printf("[+] Launched with options ... port : %i - threads used : %i - size of files (in bytes) : %i\n", port, nbThreads, fileSize);

  // Initiate an array of 1000 files
  initFiles(fileSize);

  // Initiate the server
  int sockfd = initSocketServer(port, nbThreads);

  // Listen to client's requests
  // TO DO : Don't know the size of the key yet but should be smaller than the size of the file so "sizeof(uint8_t)*fileSize*fileSize"
  size_t requestSize = sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * fileSize * fileSize);
  uint8_t *request = (uint8_t *)malloc(requestSize);
  while (!end)
  {
    int newsockfd = saccept(sockfd);
    int nbRead = sread(newsockfd, &(*request), requestSize);
    while (nbRead > 0)
    {
      // Retreive key informations from the request
      uint32_t fileIndex = *(uint32_t *) request;
      uint32_t keySize = *(uint32_t *)(request + 4);
      uint8_t *key = malloc(sizeof(uint8_t) * keySize);
      key = (uint8_t *)(request + 8);

      // Encrypte file with key
      uint8_t ** keyMatrix = arrayToMatrix(key, keySize);
      uint8_t ** encryptedMatrix = encryptFile(files[fileIndex], fileSize, keyMatrix, keySize);
      uint8_t * encryptedArray = matrixToArray((void **) encryptedMatrix, fileSize);

      // Generate response
      size_t responseSize = sizeof(uint8_t) + sizeof(uint32_t) + fileSize*fileSize*sizeof(uint8_t);
      uint8_t * response = (uint8_t *) malloc(responseSize);
      *(uint8_t *)response = 0; // The error code
      *(uint32_t *)(response+1) = fileSize*fileSize;
      for (int i = 5; i < (fileSize*fileSize)+5; i++)
      {
        response[i] = encryptedArray[i-5];
      }
      display(response, responseSize);
      swrite(newsockfd, *(&response), responseSize);

      nbRead = sread(newsockfd, *(&request), requestSize);
    }
  }

  // Close the connection
  sclose(sockfd);
  return 0;
}
