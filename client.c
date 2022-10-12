#include "utils.h"

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

uint8_t * generateRequest(uint32_t index, uint32_t size) {
    printf("[+] Generating request ...\n");
    // 4 bytes : index, 4 bytes : size of the key, N*N bytes : key
    uint8_t * request = (uint8_t *) malloc(sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t)*size*size));
    *(uint32_t *)request = index;
    *(uint32_t *) (request+4) = size;
    for(int i=8; i<(size*size)+8; i++) {
        request[i] = i-8;
    }
    return request;
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

    int s = 0; //seconds
    while(s < sec && !end){
        int i = 0;
        while(i < req){
            // Generate request
            uint8_t * request = generateRequest(i, keyBytes);
            swrite(sockfd, &(*request), sizeof(uint32_t) + sizeof(u_int32_t) + (sizeof(uint8_t)*keyBytes*keyBytes));
            i++;
        }
        sleep(1);
        s++;
    }
    return EXIT_SUCCESS;
}


