#include "utils.h"

#define BUFFER_SIZE 9
#define NPAGES 1000
#define ARRAY_TYPE uint32_t
#define MAX 1000

/***************************************
 *              GLOBALS
 ***************************************/

int sockfd;
int port;
char *server;
int keysz;

int initSocketClient(char *localhost, int port)
{
	int socketfd = ssocket();
	printf("[+] Server socket created successfully.\n");
	sconnect(localhost, port, socketfd);
	printf("[+] Connected to Server.\n");
	return socketfd;
}

void *rcv(void *r)
{
	int ret;
	int sockfd;
	int receive_times[MAX];
	ARRAY_TYPE *key;
	key = malloc(keysz * keysz * sizeof(ARRAY_TYPE));

	// Creating socket file descriptor
	sockfd = ssocket();
	sconnect(server, port, sockfd);

	// Send file id
	unsigned fileindex = htonl(rand() % NPAGES);
	ret = send(sockfd, &fileindex, 4, 0);

	// Send key size
	int revkey = htonl(keysz);
	ret = send(sockfd, &revkey, 4, 0);

	// Send key
	ret = send(sockfd, key, sizeof(ARRAY_TYPE) * keysz * keysz, 0);

	unsigned char error;
	recv(sockfd, &error, 1, 0);

	unsigned filesz;
	recv(sockfd, &filesz, 4, 0);

	if (filesz > 0)
	{
		long int left = ntohl(filesz);
		char buffer[65536];
		while (left > 0)
		{
			unsigned b = left;
			if (b > 65536)
				b = 65536;
			left -= recv(sockfd, &buffer, b, 0);
		}
	}

	unsigned t = (unsigned)(intptr_t)r;
	receive_times[t] = getts();
	close(sockfd);
}

int main(int argc, char **argv)
{

	// Handle the args
	keysz = atoi(argv[2]);
	int rate = atoi(argv[4]);
	int time = atoi(argv[6]);
	char *line = argv[7];
	char *search = ":";
	server = strtok(line, search);
	port = atoi(strtok(NULL, search));
	printf("[+] Launched with options ... size of the key : %d  -  requests/second : %d  -  time of execution (in seconds) : %d  -  ip address : %s -   tcp port : %d \n", keysz, rate, time, server, port);

	int diffrate = 1000000000 / rate;
	int i = 0;
	int start = 0;
	int next = 0;
	int sent_times[MAX];
	while (getts() - start < (long unsigned)1000000000 * time)
	{
		next += diffrate;
		while (getts() < next)
		{
			usleep((next - getts()) / 1000);
		}

		sent_times[i] = getts();
		pthread_t thread;
		pthread_create(&thread, NULL, rcv, (void *)(intptr_t)i);
		i++;
	}

	return EXIT_SUCCESS;
}
