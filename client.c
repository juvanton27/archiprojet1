#include "utils.h"
#include "request.h"

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

    // struct request going to be send randomly

    Request request;
    int i = 0;
    int nbytes;
    while(i < req){
        request.index = i;
        request.key = key;
        if ((nbytes = write(sockfd, &request, sizeof(Request)) != sizeof(Request))){
             printf("error sending request");
        }
        i++;
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


