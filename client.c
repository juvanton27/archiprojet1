#include "utils.h"
#include "request.h"
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 10000
#define LOCAL_HOST "127.0.0.1"


volatile sig_atomic_t end = 0;

int sockfd;

void endClientHandler(int sig) {
    printf("[+] Stoping client ...\n");
    end = 1;
}


// SOCKET HANDLER
int initSocketClient(char* localhost, int port){
    int socketfd = ssocket();
    printf("[+] Server socket created successfully.\n");
    sconnect(localhost, port, socketfd);
    printf("[+] Connected to Server.\n");
    return socketfd;
}

int main(int argc, char **argv) {
    ssigaction(SIGINT, endClientHandler);

    char* server;
    int port;
    int keyBytes = atoi(argv[2]);
    int req = atoi(argv[4]);
    int sec = atoi(argv[6]);
    char* line = argv[7];
    char *search = ":";

    // server will point to 127.0.0.1
    server = strtok(line, search);
 
    // port will point to 2241 for example.
    port = atoi(strtok(NULL, search));

    printf("KEY SIZE : %d  -  REQUESTS/SECONDS : %d  -  SECONDS : %d  -  SERVER : %s -   TCP PORT : %d \n", keyBytes, req, sec, server,port);

    // Socket
    sockfd = initSocketClient(server, port);

    // Generate key
    printf("[+] Generating key ...\n");
    uint8_t **key;
    key = (uint8_t**) malloc(keyBytes*sizeof(uint8_t*));
    for(int row=0; row<keyBytes; row++) {
        key[row] = (uint8_t*) malloc(keyBytes*sizeof(uint8_t));
        for(int col=0; col<keyBytes; col++) {
            key[row][col] = 0;
        }
    }

    // struct request going to be send randomly
    Request request;
    
    int nbytes;
    int s = 0; //seconds

    while(s < sec && !end){
        int i = 0;
        while(i < req){
            request.index = i;
            request.size = keyBytes*keyBytes;
            request.key = key;
            printf("index sent: %i %i\n", request.index, request.size);
            if ((nbytes = swrite(sockfd, &request, sizeof(Request)) != sizeof(Request))){
                printf("error sending request");
            }
            i++;
        }
        sleep(1);
        s++;
    }  
    
    
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! A METTRE DANS SERVER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Request received_request;
    int size;

    if( (size = recv ( sockfd, (void*)&received_request, sizeof(Request), 0)) >= 0)
    {
     // check the size
    }
    */
    return EXIT_SUCCESS;
}


