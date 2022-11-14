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
ARRAY_TYPE *key;

unsigned receive_times[MAX];

int counter = 0;

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
	counter += 1;
	int ret;
	int sockfd;

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

	struct timeval start;
	gettimeofday(&start, NULL);

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
	struct timeval end;
	gettimeofday(&end, NULL);
	receive_times[t] = (end.tv_sec*1000000)+end.tv_usec-(start.tv_sec*1000000)+start.tv_usec;

	sclose(sockfd);
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

	key = malloc(keysz * keysz * sizeof(ARRAY_TYPE));
	for (int i = 0; i < keysz * keysz; i++)
	{
		key[i] = i;
	}

	unsigned sent_times[MAX];

	unsigned long diffrate = 1000000 / rate;
	int i = 0;

	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL);

	unsigned long next = ((start.tv_sec * 1000000) + start.tv_usec);
	gettimeofday(&end, NULL);
	while (((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec) < (long unsigned)time * 1000000)
	{
		next += diffrate;
		while (((end.tv_sec * 1000000) + end.tv_usec) < next)
		{
			usleep(next - ((end.tv_sec * 1000000) + end.tv_usec));
			gettimeofday(&end, NULL);
		}

		sent_times[i] = (end.tv_sec*1000000)+end.tv_usec-(start.tv_sec*1000000)+start.tv_usec;
		pthread_t thread;
		pthread_create(&thread, NULL, rcv, (void *)(intptr_t)i);
		i++;
		gettimeofday(&end, NULL);
	}

	char receive_title[100];
	char send_title[100];
	printf("oui\n");
	sprintf(receive_title, "receive_time_key%d-rate%d-time%d.csv", keysz, rate, time);
	sprintf(send_title, "send_time_key%d-rate%d-time%d.csv", keysz, rate, time);

	int fd1 = sopen(receive_title, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	int fd2 = sopen(send_title, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	char buf[10];

	for(int i=0; i<MAX; i++) {
		sprintf(buf, "%d;%d\n", i, receive_times[i]);
		swrite(fd1, buf, strlen(buf));
		if(i>1) {
			sprintf(buf, "%d;%d\n", i, sent_times[i]-sent_times[i-1]);
		} else {
			sprintf(buf, "%d;%d\n", i, sent_times[i]);
		}
		swrite(fd2, buf, strlen(buf));
	}

	sclose(fd1);
	sclose(fd2);

	return EXIT_SUCCESS;
}
