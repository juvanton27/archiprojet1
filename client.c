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

int main(int argc, char **argv) {
    ssigaction(SIGINT, endClientHandler);

    char* server;
    int port;
    int key = atoi(argv[2]);
    int req = atoi(argv[4]);
    int sec = atoi(argv[6]);
    char* line = argv[7];
    char *search = ":";

    // server will point to 127.0.0.1
    server = strtok(line, search);
 
    // port will point to 2241 for example.
    port = atoi(strtok(NULL, search));

    printf("KEY SIZE : %d  -  REQUESTS/SECONDS : %d  -  SECONDS : %d  -  SERVER : %s -   TCP PORT : %d \n", key, req, sec, server,port);

    // Socket
    sockfd = initSocketClient(server, port);

    // Prompt
    char buf[BUFFER_SIZE];
    int val;
    int nbCharRd = sread(0, &buf, BUFFER_SIZE);
    while(nbCharRd > 0) {
        val = atoi(buf);
        swrite(sockfd, &val, sizeof(int));
        nbCharRd = sread(0, &buf, BUFFER_SIZE);
    }

    return EXIT_SUCCESS;
}


