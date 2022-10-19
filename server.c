#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

#include "utils.h"

#define NB_FILES 1000

/*****************************
 *          GLOBALS          *
 *****************************/

int sockfd, newsockfd;
volatile uint8_t **files[NB_FILES];
int port, nbThreads, fileSize;
volatile sig_atomic_t end = 0;

volatile sig_atomic_t error = 0;

typedef struct Key
{
  uint32_t fileIndex;
  uint32_t keySize;
  uint8_t **keyMatrix;
} Key;

Key keyQueue[10*NB_FILES];
int keyCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

/***************************************
 *              SERVER
 ***************************************/

void endServerHandler(int sig)
{
  printf("[+] End of the server ...\n");
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

/*****************************
 *           FILES           *
 *****************************/

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
  uint8_t **result = (uint8_t **)malloc(fileSize * sizeof(uint8_t *));
  for (int i = 0; i < fileSize; i++)
  {
    result[i] = (uint8_t *)malloc(fileSize * sizeof(uint8_t));
  }
  for (int i = 0; i < keySize; i++)
  {
    for (int j = 0; j < keySize; j++)
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

void handleEncryptError(int sig)
{
  error = 1;
}

/*****************************
 *           MATRIX          *
 *****************************/

uint8_t **arrayToMatrix(void *array, int size)
{
  uint8_t *arr = (uint8_t *)array;
  uint8_t **matrix = (uint8_t **)malloc(sizeof(uint8_t *) * size);
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

uint8_t *matrixToArray(void **matrix, int size)
{
  uint8_t **mat = (uint8_t **)matrix;
  uint8_t *array = (uint8_t *)malloc(sizeof(uint8_t) * size * size);
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      array[(i * size) + j] = mat[i][j];
    }
  }
  return array;
}

/*****************************
 *          THREADS          *
 *****************************/
void executeKey(Key *key)
{
  ssigaction(SIGSEGV, handleEncryptError);

  uint8_t **encryptedMatrix = encryptFile(files[key->fileIndex], fileSize, key->keyMatrix, key->keySize);
  uint8_t *encryptedArray = matrixToArray((void **)encryptedMatrix, fileSize);

  // // Generate response
  // size_t responseSize = sizeof(uint8_t) + sizeof(uint32_t) + fileSize * fileSize * sizeof(uint8_t);
  // uint8_t *response = (uint8_t *)malloc(responseSize);
  // *(uint8_t *)response = 0; // The error code
  // *(uint32_t *)(response + 1) = fileSize;
  // for (int i = 5; i < (fileSize * fileSize) + 5; i++)
  // {
  //   response[i] = encryptedArray[i - 5];
  // }

  // if(error) {
  //   printf("ERR: %i\n", 1);
  //   error = 0;
  // }

  printf("ERR: %i, size: %i", 0, fileSize);
  // display(response, responseSize);

  free(encryptedMatrix);
  free(encryptedArray);
}

void submitKey(Key key)
{
  pthread_mutex_lock(&mutexQueue);
  keyQueue[keyCount++] = key;
  pthread_mutex_unlock(&mutexQueue);
  pthread_cond_signal(&condQueue);
}

void *startThread(void *args)
{
  while (1)
  {
    Key key;

    pthread_mutex_lock(&mutexQueue);
    while (keyCount == 0)
    {
      pthread_cond_wait(&condQueue, &mutexQueue);
    }

    key = keyQueue[0];
    for (int i = 0; i < keyCount - 1; i++)
    {
      keyQueue[i] = keyQueue[i + 1];
    }
    keyCount--;
    pthread_mutex_unlock(&mutexQueue);
    executeKey(&key);
  }
}

/*****************************
 *        OPTIONS
 *****************************/

void handleOptions(int argc, char **argv)
{
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
}

int main(int argc, char **argv)
{
  // Handle Ctrl+C
  ssigaction(SIGINT, endServerHandler);

  // Handle the args
  handleOptions(argc, argv);
  printf("[+] Launched with options ... port : %i - threads used : %i - size of files (in bytes) : %i\n", port, nbThreads, fileSize);

  // Initiate an array of 1000 files
  initFiles(fileSize);

  // Initiate the server
  sockfd = initSocketServer(port, nbThreads);

  // Handle threads
  pthread_t *threads = (pthread_t *)malloc(nbThreads * sizeof(pthread_t));
  pthread_mutex_init(&mutexQueue, NULL);
  pthread_cond_init(&condQueue, NULL);

  for (int i = 0; i < nbThreads; i++)
  {
    if (pthread_create(&threads[i], NULL, &startThread, NULL) != 0)
    {
      perror("Failed to create the thread");
    }
  }

  while (!end)
  {
    size_t requestSize = sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t) * fileSize * fileSize);
    uint8_t *request = (uint8_t *)malloc(requestSize);
    newsockfd = saccept(sockfd);
    int nbRead = sread(newsockfd, &(*request), requestSize);
    while (nbRead > 0)
    {
      // Retreive key informations from the request
      uint32_t fileIndex = *(uint32_t *)request;
      uint32_t keySize = *(uint32_t *)(request + 4);
      uint8_t *keyMatrix = malloc(sizeof(uint8_t) * keySize);
      keyMatrix = (uint8_t *)(request + 8);
      uint8_t **matrix = arrayToMatrix(keyMatrix, keySize);
      Key key = {
          .fileIndex = fileIndex,
          .keySize = keySize,
          .keyMatrix = matrix};

      submitKey(key);

      nbRead = sread(newsockfd, &(*request), requestSize);
    }
    sclose(newsockfd);
    free(request);
  }

  for (int i = 0; i < nbThreads; i++)
  {
    if (pthread_join(threads[i], NULL) != 0)
    {
      perror("Failed to join the thread");
    }
  }
  pthread_mutex_destroy(&mutexQueue);
  pthread_cond_destroy(&condQueue);

  free(threads);

  return 0;
}
