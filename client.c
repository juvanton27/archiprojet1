#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <stdarg.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#define BUFFER_SIZE 10000
#define LOCAL_HOST "127.0.0.1"



volatile sig_atomic_t end = 0;
volatile sig_atomic_t toSend = 0;

int sockfd;
int delay;

void sigint_handler(int sig) {
    end =1;
}


// SOCKET HANDLER
int initSocketClient(char* localhost, int port){
    int socketfd = ssocket();
    printf("[+]Server socket created successfully.\n");
    sconnect(localhost, port, socketfd);
    printf("[+]Connected to Server.\n");
    return socketfd;
}

ssize_t sread(int fd, void *buf, size_t count) {
  ssize_t r = read(fd, buf, count);
  checkNeg(r , "Error READ");
  return r;
}

int sclose(int fd) {
  int resv = close(fd);
  checkNeg(resv, "Error CLOSE");
  return resv;
}

void ssigaction(int signum, void (*handler)(int signum)) {
  struct sigaction action;
  action.sa_handler = handler;
  ssigfillset(&action.sa_mask);
  action.sa_flags = 0;
  
  int r = sigaction (signum, &action, NULL);
  checkNeg(r, "Error sigaction");
}

void ssigemptyset(sigset_t *set) {
  int res = sigemptyset(set);
  checkNeg(res, "sigemptyset");
}


void ssigaddset(sigset_t *set, int signum) {
  int res = sigaddset(set, signum);
  checkNeg(res, "sigaddset");
}

int main(int argc, char **argv) {
    ssigaction(SIGINT, sigint_handler);
    sigset_t set;
    ssigemptyset (&set);
    ssigaddset(&set, SIGINT);

    // Arguments
    //char* server = argv[1];
    //int port = atoi(argv[2]);
    //delay = atoi(argv[3]);
    //int num = atoi(argv[4]);
    //printf("SERVER : %s  -  PORT : %d  -  DELAY : %d  -  NUM : %d  \n", server, port, delay, num);
    char* server;
    int port;
    int key = atoi(argv[2]);
    int req = atoi(argv[4]);
    int sec = atoi(argv[6]);
    char *token;
    char line[] = argv[7];
    char *search = ":";

    // server will point to 127.0.0.1
    server = strtok(line, search);

 
    // port will point to 2241 for example.
    port = strtok(NULL, search);


    printf("KEY SIZE : %d  -  REQUESTS/SECONDS : %d  -  SECONDS : %d  -  SERVER : %s -   TCP PORT : %d \n", key, req, sec, server,port);

    // Socket
    sockfd = initSocketClient(server, port);


    // Prompt
    char buf[BUFFER_SIZE];
    int nbCharRd = sread(0, buf, BUFFER_SIZE);

    while(nbCharRd > 0) {
        // Command split
        char cmd = '\0';
        switch (cmd)
        {
        case 'q':
            // Close connection
            sclose(sockfd);
            return EXIT_SUCCESS;
        default:
            return EXIT_FAILURE;
        }
        nbCharRd = sread(0, buf, BUFFER_SIZE);
    }
    return EXIT_SUCCESS;
}


