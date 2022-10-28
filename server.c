#include "utils.h"

#define NPAGES 1000
#define ARRAY_TYPE uint32_t

void ** pages;
int opt, nthreads, nbytes, port, verbose, client_sock;
uint8_t err;

int connection_handler(void *socket_desc) {
  int fileid, keysz;
  //Get the socket descriptor
  int sockfd = (int)(intptr_t)socket_desc;

  int tread = recv(sockfd, &fileid, 4, 0);
  tread = recv(sockfd, &keysz, 4, 0); //Network byte order
  keysz = ntohl(keysz);
  ARRAY_TYPE * key;
  key = malloc(keysz*keysz*sizeof(ARRAY_TYPE));
  unsigned tot = keysz*keysz * sizeof(ARRAY_TYPE); 
  unsigned done = 0;
  while (done < tot) {
    tread = recv(sockfd, key, tot- done, 0);
    done += tread;
  }

  int nr = nbytes / keysz;
  ARRAY_TYPE* file = pages[fileid % NPAGES];
  ARRAY_TYPE* crypted = malloc(nbytes*nbytes * sizeof(ARRAY_TYPE));
  //Compute sub-matrices
  for (int i = 0; i < nr ; i ++) { 
    int vstart = i * keysz;
    for (int j = 0; j < nr; j++) {
      int hstart = j * keysz;
      //Do the sub-matrix multiplication
      for (int ln = 0; ln < keysz; ln++) {
        int aline = (vstart + ln) * nbytes + hstart;
        for (int col = 0; col < keysz; col++) {
          int tot = 0;
          for (int k = 0; k < keysz; k++) {
            int vline = (vstart + k) * nbytes + hstart;
            tot += key[ln * keysz + k] * file[vline + col]; 
          }
          crypted[aline + col] = tot; 
        }
      } 
    }
  }
  send(sockfd, &err, 1, MSG_NOSIGNAL);
  unsigned sz = htonl(nbytes*nbytes*sizeof(ARRAY_TYPE));
  send(sockfd, &sz, 4, MSG_NOSIGNAL);
  send(sockfd, crypted, nbytes*nbytes*sizeof(ARRAY_TYPE), MSG_NOSIGNAL);

  free(key);
  free(crypted);
  sclose(sockfd);

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  int sockfd;

  while ((opt = getopt(argc, argv, "j:s:p:v")) != -1) { 
    switch (opt) {
      case 'j': 
        nthreads = atoi(optarg);
        break; 
      case 's': 
        nbytes = atoi(optarg); 
        break; 
      case 'p': 
        port = atoi(optarg); 
        break; 
      case 'v': 
        verbose = 1; 
        break;
      default:
        fprintf(stderr, "Usage: %s [-j threads] [-s bytes] [-p port]\n", argv[0]); 
    }
  }

  // Creating socket file descriptor
  sockfd = ssocket();
  sbind(port, sockfd);

  // Allocating files
  pages = malloc(sizeof(void*)*NPAGES);
  for(int i=0; i<NPAGES; i++) {
    pages[i] = malloc(nbytes*nbytes*sizeof(ARRAY_TYPE));
  }

  printf("Listening...\n");
  if ((slisten(sockfd, 128)) != 0) { 
    printf("Listen failed...\n");
    exit(1); 
  } else printf("Server listening..\n");

  while((client_sock = saccept(sockfd)))
  {
    connection_handler((void*)(intptr_t)client_sock);
  }

  return EXIT_SUCCESS;
}
